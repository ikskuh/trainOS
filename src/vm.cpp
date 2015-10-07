#include <kstdlib.h>
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

extern "C" void vm_start()
{

/*
}

void code()
{
//*/

    struct {
        const char *ptr;
        uint32_t size;
    } mainAssembly {
        &mainscript_start,
        (uint32_t)&mainscript_size
    };

    VirtualMachine vm;
    vm.import("print") = printArguments;

    Assembly *assembly = vm.load(mainAssembly.ptr, mainAssembly.size);
    if(assembly == nullptr) {
        kprintf("failed to load assembly :(\n");
        return;
    }

    /*
    kprintf("Assembly:\n");
    kprintf("  Name:        %s\n", assembly->name().str());
    kprintf("  Author:      %s\n", assembly->author().str());
    kprintf("  Description: %s\n", assembly->description().str());
    //*/
    /*
    kprintf("Type list:\n");
    for(const auto &type : vm.types()) {
        kprintf("%s: %x\n", type.first.str(), vm.type(type.first));
    }
    //*/

    Process *process = vm.createProcess(assembly);
    if(process == nullptr) {
        kprintf("Failed to create process.\n");
        return;
    }


    while(vm.step())
    {
        // kprintf(".");
    }

    process->release();
    assembly->release();

    kprintf("\n");
}

