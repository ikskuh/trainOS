#include <stdlib.h>
#include <timer.h>
#include <dynamic.h>
#include <console.h>

#include <virtualmachine.hpp>
#include <vmtype.hpp>
#include <types/vmprimitivetype.hpp>

extern "C" {
	extern const char mainscript_start;
	extern const char mainscript_end;
	extern const char mainscript_size;
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

extern "C" void vm_start()
{
	struct {
		const char *ptr;
		uint32_t size;
	} mainfile {
		&mainscript_start,
		(uint32_t)&mainscript_size
	};

    VirtualMachine vm;
    vm.import("print") = printArguments;


    while(vm.step())
    {
        kprintf(".");
    }

    kprintf("\n");
}

