#include <stdlib.h>
#include "../trainscript/tsvm.hpp"

using namespace ker;
using namespace trainscript;

extern "C" {
//    extern const char file01_start;
//    extern const char file01_end;
//    extern size_t file01_size;
}

char file01[] = "VAR global : INT;\0";

String fn()
{
    return String("keks");
}

extern "C" void vm_start()
{
    kprintf("Testing ker::String:\n");

    {
        String a("hello");
        String b(a);
        String c;
        String d(fn());
        String e;
        c = a;
        e = fn();

        kprintf("'%s' '%s' '%s' '%s' '%s'\n", a.str(), b.str(), c.str(), d.str(), e.str());
    }


    kprintf("Parse kernel module:");
    Module *module = VM::load(file01, sizeof(file01));
    if(module == nullptr) {
        kprintf("Could not load module :(\n");
        return;
    }

    kprintf("Module successfully loaded :)\n");

    for(const auto &var : module->variables)
    {
        kprintf("Variable: '%s' => '");
        var.second->printval();
        kprintf("\n");
    }
}
