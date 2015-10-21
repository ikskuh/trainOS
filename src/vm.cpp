#include <kstdlib.h>
#include <timer.h>
#include <dynamic.h>
#include <console.h>
#include <multiboot.h>

#include <virtualmachine.hpp>
#include <process.hpp>
#include <thread.hpp>

#include <vmtype.hpp>
#include <types/vmprimitivetype.hpp>

#include <interrupts.h>

#include "../csl/cpustatetype.hpp"
#include "../csl/io.hpp"

// Some static assertions
static_assert(sizeof(uint32_t) == sizeof(void*), "void* is not 32 bit wide.");

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
		// TODO: No, don't die!
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

		using DriverProcess = ker::Pair<Process*,uint32_t>;
		ker::Vector<DriverProcess> drivers;

		const MultibootModule *mod = (const MultibootModule *)mbHeader->modules;
		for(size_t i = 0; i < mbHeader->moduleCount; i++)
		{
			kprintf("Loading Module '%s'...\n", (const char*)mod[i].name);

			size_t len = mod[i].end - mod[i].start;

            Module *module = machine.load(reinterpret_cast<void*>(mod[i].start), len);
            if(module == nullptr) {
				die("Failed to load assembly.");
				return;
			}
            switch(module->type())
			{
				case AssemblyType::Library:
				{
					// Assemblies don't need special treatment
					break;
				}
				case AssemblyType::Executable:
				{
                    Process *program = machine.createProcess(module, false);
					if(program == nullptr) {
						die("Failed to create process.");
						return;
					}
					program->release();
					break;
				}
				case AssemblyType::Service:
				{
                    Process *program = machine.createProcess(module, true);
					if(program == nullptr) {
						die("Failed to create service process.");
						return;
					}
					program->release();
					break;
				}
				case AssemblyType::Driver:
				{
                    Process *program = machine.createProcess(module, true);
					if(program == nullptr) {
						die("Failed to create driver process.");
						return;
                    }
                    if(module->exports().contains("irq")) {
                        drivers.append({ program, module->exports()["irq"] });
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
            module->release();
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

