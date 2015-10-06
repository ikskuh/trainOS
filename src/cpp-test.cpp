#include "console.h"
#include <ker/string.hpp>
#include <ker/vector.hpp>
#include <ker/dictionary.hpp>

using namespace ker;

String strfn()
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
        String d(strfn());
        String e;
        c = a;
        e = strfn();

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
