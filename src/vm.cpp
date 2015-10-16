#include <kstdlib.h>
#include <timer.h>
#include <dynamic.h>
#include <console.h>
#include <multiboot.h>

#include <virtualmachine.hpp>
#include <vmtype.hpp>
#include <types/vmprimitivetype.hpp>

#include <interrupts.h>

#include "../csl/cpustatetype.hpp"
#include "../csl/io.hpp"
#include "../csl/casts.hpp"

void printVMValue(const VMValue &value)
{
    switch(value.type().type()) {
        case VMTypeType::Void:
            kprintf("void");
            break;
        case VMTypeType::Compound:
            kprintf("compound");
            break;
        case VMTypeType::Pointer:
            kprintf("pointer");
            break;
        case VMTypeType::Primitive:
        {
            VMText str = static_cast<const VMPrimitiveType &>(value.type()).toString(value);
            kprintf("%s", str.str());
            break;
        }
        default:
            kprintf("???");
            break;
    }
}

ExceptionCode printArguments(VMValue &, const VMArray &args)
{
    for(const VMValue &arg : args) {
        printVMValue(arg);
    }
    return ExceptionCode::None;
}

struct dtortest {
    void *mem;

	dtortest() : mem(malloc(42)) { kprintf("[alloc]"); }
	~dtortest() { free(mem); kprintf("[free]"); }
} ;// object;

struct IrqList {
    static const size_t length = 64;
    volatile uint32_t read;
    volatile uint32_t write;
    CpuState items[IrqList::length];
} irqFiFo {
    0, 0, {}
};

extern "C" void vm_handle_interrupt(CpuState *state)
{
    irqFiFo.items[irqFiFo.write] = *state;
    irqFiFo.write += 1;
	if(irqFiFo.write >= irqFiFo.length) {
		// TODO: Don't die!
		// Logging, statistics...
		die("irqFiFo overflow.");
	}
}


const char *execptionName(ExceptionCode ex)
{
    switch(ex) {
#define CASE(ex) case ExceptionCode::ex: return #ex
        CASE(None);
        CASE(Terminated);
        CASE(InvalidOpcode);
        CASE(InvalidOperands);
        CASE(StackUnderflow);
        CASE(StackOverflow);
        CASE(DivideByZero);
        CASE(InvalidJump);
        CASE(CallrWithoutResult);

        CASE(InvalidOperator);
        CASE(InvalidType);
        CASE(InvalidGlobal);
        CASE(NullPointer);
        CASE(InvalidMember);
		CASE(InvalidArgument);
#undef CASE
        default: return "Unknown";
    }
}

static bool shutdownRequested = false;

ExceptionCode shutdown(VMValue &, const VMArray &)
{
	shutdownRequested = true;
	return ExceptionCode::None;
}

int ReferenceCounted::counter = 0;

extern "C" void vm_start(const MultibootStructure *mbHeader)
{
	// intr_set_handler(0x20, vm_handle_interrupt);
    intr_set_handler(0x21, vm_handle_interrupt);

	{
		VirtualMachine machine;
		machine.type("CPUSTATE") = csl::CpuStateType;
		machine.import("print") = printArguments;
		machine.import("shutdown") = shutdown;

		machine.import("inb") = csl::inb;
		machine.import("outb") = csl::outb;

		machine.import("toInt8") = csl::toInt8;
		machine.import("toInt16") = csl::toInt16;
		machine.import("toInt32") = csl::toInt32;
		machine.import("toUInt8") = csl::toUInt8;
		machine.import("toUInt16") = csl::toUInt16;
		machine.import("toUInt32") = csl::toUInt32;

		using DriverProcess = ker::Pair<Process*,uint32_t>;
		ker::Vector<DriverProcess> drivers;

		const MultibootModule *mod = (const MultibootModule *)mbHeader->modules;
		for(size_t i = 0; i < mbHeader->moduleCount; i++)
		{
			kprintf("Loading Module '%s'...\n", (const char*)mod[i].name);

			size_t len = mod[i].end - mod[i].start;

			Assembly *assembly = machine.load(reinterpret_cast<void*>(mod[i].start), len);
			if(assembly == nullptr) {
				die("Failed to load assembly.");
				return;
			}
			switch(assembly->type())
			{
				case AssemblyType::Library:
				{
					// Assemblies don't need special treatment
					break;
				}
				case AssemblyType::Executable:
				{
					Process *program = machine.createProcess(assembly, false);
					if(program == nullptr) {
						die("Failed to create process.");
						return;
					}
					program->release();
					break;
				}
				case AssemblyType::Service:
				{
					Process *program = machine.createProcess(assembly, true);
					if(program == nullptr) {
						die("Failed to create service process.");
						return;
					}
					program->release();
					break;
				}
				case AssemblyType::Driver:
				{
					Process *program = machine.createProcess(assembly, true);
					if(program == nullptr) {
						die("Failed to create driver process.");
						return;
					}
					if(assembly->exports().contains("irq")) {
						drivers.append({ program, assembly->exports()["irq"] });
					} else {
						program->release();
					}
					break;
				}
				default:
				{
					die("OS.UnknownAssemblyType");
					break;
				}
			}
			assembly->release();
		}

		while((shutdownRequested == false) && machine.step())
		{
			// Remove all terminated drivers
			for(size_t i = 0; i < drivers.length(); ) {
				if(drivers[i].first->isRunning()) {
					i++;
				} else {
					drivers[i].first->release();
					drivers.remove(i);
				}
			}

			// check for IRQ requests
			do
			{
				// atomic checking for existing IRQ item
				irq_disable();
				bool hasItem = (irqFiFo.read < irqFiFo.write);
				irq_enable();

				if(hasItem == false) {
					break; // we don't have anything to read
				}

				CpuState *cpu = &irqFiFo.items[irqFiFo.read];

				for(const auto &driver : drivers) {
					Thread *thread = driver.first->createThread(driver.second);
					thread->start({
						VMValue::Int32(cpu->intr),
						csl::createCpuState(cpu)
					});
					thread->release();
				}
				irqFiFo.read += 1;

				irq_disable();
				// When fifo is emptied, reset list pointers
				if(irqFiFo.read == irqFiFo.write) {
					irqFiFo.read = 0;
					irqFiFo.write = 0;
				}
				irq_enable();
			} while(true);
		}
	}
    kprintf("\n");
	kprintf("unreleased objects: %d\n", ReferenceCounted::counter);
	kprintf("\n");
}

