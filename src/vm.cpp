#include <kstdlib.h>
#include <timer.h>
#include <dynamic.h>
#include <console.h>

#include <virtualmachine.hpp>
#include <vmtype.hpp>
#include <types/vmprimitivetype.hpp>

#include <interrupts.h>

#include "../csl/cpustatetype.hpp"

extern "C" {
	extern const char mainscript_start;
	extern const char mainscript_end;
	extern const char mainscript_size;

    extern const char firstrun_start;
    extern const char firstrun_end;
    extern const char firstrun_size;
}

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

    dtortest() : mem(malloc(42)) { }
    ~dtortest() { free(mem); }
};


struct {
    const char *ptr;
    uint32_t size;
} mainAssembly {
    &mainscript_start,
    (uint32_t)&mainscript_size
};

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

extern "C" void vm_start()
{
	// intr_set_handler(0x20, vm_handle_interrupt);
    intr_set_handler(0x21, vm_handle_interrupt);

    VirtualMachine machine;
	machine.type("CPUSTATE") = csl::CpuStateType;
    machine.import("print") = printArguments;

    Assembly *assembly = machine.load(mainAssembly.ptr, mainAssembly.size);
    if(assembly == nullptr) {
        kprintf("failed to load assembly :(\n");
        return;
    }

    Process *irqService = machine.createProcess(assembly, true);
    if(irqService == nullptr) {
        kprintf("Failed to create process.\n");
        return;
    }

    uint32_t irqRoutine = irqService->assembly()->exports()["irq"];

    while(machine.step())
    {
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

            auto *thread = irqService->createThread(irqRoutine);
			thread->start({
				VMValue::Int32(cpu->intr),
				csl::createCpuState(cpu)
			});

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

    irqService->release();
    irqService = nullptr;

    assembly->release();

    kprintf("\n");
}

