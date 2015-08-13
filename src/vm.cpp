#include <stdlib.h>
#include "../trainscript/tsvm.hpp"

using namespace ker;
using namespace trainscript;

extern "C" {
//    extern const char file01_start;
//    extern const char file01_end;
//    extern size_t file01_size;
}

char file01[] = R"code(
VAR global : INT;
PUB main() | i : INT
BEGIN
    print(10, 20, 30);
    0 -> i;
    REPEAT
    BEGIN
        print(i);
        i + 1 -> i;
    END
END)code";

void cpp_test();

class PrintMethod :
        public Method
{
public:
    Variable invoke(Vector<Variable> arguments) override;
};

Variable PrintMethod::invoke(Vector<Variable> arguments)
{
    for(auto &var : arguments)
    {
        var.printval();
        kprintf(" ");
    }
    kprintf("\n");
    return Variable::Void;
}

extern "C" void vm_start()
{
    // cpp_test();

    kprintf("Parse kernel module: ");
    Module *module = VM::load(file01, sizeof(file01) - 1);
    if(module == nullptr) {
        kprintf("Could not load module :(\n");
        return;
    }

    kprintf("Module successfully loaded :)\n");

    module->methods.add("print", new PrintMethod());

    for(const auto &var : module->variables)
    {
        kprintf("Variable: '%s' => '", var.first.str());
        var.second->printval();
        kprintf("'\n");
    }

    for(const auto &fn : module->methods)
    {
        kprintf("Function: '%s'\n", fn.first.str());
    }

    Method *main = module->method("main");
    if(main == nullptr) {
        kprintf("Script has no main method.\n");
        return;
    }

    main->invoke({ });

    delete module;
}


String fn()
{
    return String("keks");
}

void put(Vector<int> &v)
{
    kprintf("v[%d]: ", v.length());
    for(int i : v) { kprintf("%d,", i); }
    kprintf("\n");
}

void put(Vector<String> &v)
{
    kprintf("v[%d]: ", v.length());
    for(String &s : v) { kprintf("[%s],", s.str()); }
    kprintf("\n");
}

void put(Dictionary<int, int> &d)
{
    kprintf("dict: ");
    for(auto &it : d)
    {
        kprintf("[%d -> %d] ", it.first, it.second);
    }
    kprintf("\n");
}

void put(Dictionary<String, String> &d)
{
    kprintf("dict: ");
    for(auto &it : d)
    {
        kprintf("[%s -> %s] ", it.first.str(), it.second.str());
    }
    kprintf("\n");
}

void cpp_test()
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

    kprintf("Testing ker::Pair:\n");
    {
        Pair<int, int> a(10, 20);
        Pair<String, String> b("Hey", "Affe");

        Pair<int, int> c(a);
        Pair<String, String> d(b);

        Pair<int, int> e; e = a;
        Pair<String, String> f; f = b;

        kprintf("'%d,%d' '%d,%d' '%d,%d'\n", a.first, a.second, c.first, c.second, e.first, e.second);
        kprintf("'%s,%s' '%s,%s' '%s,%s'\n", b.first.str(), b.second.str(), d.first.str(), d.second.str(), f.first.str(), f.second.str());
    }

    kprintf("Testing ker::Vector:\n");
    {
        Vector<int> a;
        a.append(1);
        a.append(2);
        a.append(3);

        Vector<int> c;
        Vector<int> b(a);
        c = a;

        put(a);
        put(b);
        put(c);
    }
    {
        Vector<String> a;
        a.append("x");
        a.append("y");
        a.append("z");

        Vector<String> c;
        Vector<String> b(a);
        c = a;

        put(a);
        put(b);
        put(c);
    }

    kprintf("Testing ker::Dictionary:\n");
    {
        kprintf("int -> int\n");
        Dictionary<int, int> a;
        a.add(1, 30);
        a.add(2, 20);
        a.add(3, 10);

        kprintf("%d %d\n", a.contains(1), a.contains(4));
        kprintf("%d %d\n", a.at(1), a.at(3));
        kprintf("%d %d\n", a[1], a[3]);

        Dictionary<int, int> b(a);
        Dictionary<int, int> c;
        c = a;

        put(a);
        put(b);
        put(c);
    }

    {
        kprintf("String -> String\n");
        Dictionary<String, String> a;
        a.add("x", "30");
        a.add("y", "20");
        a.add("z", "10");

        kprintf("%d %d\n", a.contains("x"), a.contains("w"));
        kprintf("%s %s\n", a.at("x").str(), a.at("z").str());
        kprintf("%s %s\n", a["x"].str(), a["z"].str());

        Dictionary<String, String> b(a);
        Dictionary<String, String> c;
        c = a;

        put(a);
        put(b);
        put(c);
    }
}
