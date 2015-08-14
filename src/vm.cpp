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
	afraid(15, 34) → i;
	print(40, i, 60);
    0 -> i;
	WHILE i < 5 DO
    BEGIN
        print(i);
        i + 1 -> i;
    END
	REPEAT
	BEGIN

	END
END)code";

void cpp_test();

class PrintMethod :
        public Method
{
public:
    Variable invoke(Vector<Variable> arguments) override;
};

class NativeMethod :
		public Method
{
private:
	void *function;
public:
	NativeMethod(void *fn) :
		function(fn)
	{

	}

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

static void copyCode(uint8_t **ptr, const uint8_t *src, size_t length)
{
	for(size_t i = 0; i < length; i++) {
		// Copy byte
		**ptr = src[i];
		// Iterate
		(*ptr)++;
	}
}

/**
 * @brief Invokes a cdecl function
 * @param arguments Arguments passed to the c function
 * @return uint32_t return value of the c function
 */
Variable NativeMethod::invoke(Vector<Variable> arguments)
{
	if(this->function == nullptr) {
		return Variable::Invalid;
	}

	// ASM code:
	// push ebp
	// mov ebp, esp
	// (push imm32) * num
	// call [function]
	// add esp, 4 * num
	// ret

	uint8_t headerCode[] = {
		0x55, // push ebp
		0x89, 0xE5, // mov ebp, esp
	};
	uint8_t tailCode[] = {
		0xB8, 0x42, 0x00, 0x00, 0x00, // mov eax,0x42
		0xFF, 0xD0, // call eax
		0x83, 0xC4, 0x08, // add esp, 0x00
		0xC9, // leave
		// 0x5D, // pop ebp
		0xC3, // ret
	};

	// Store some memory for a function
	uint8_t pushCode[] = {
		0x68, 0x00, 0x00, 0x00, 0x00 // push 0x00
	};

	size_t stackSize = 4 * arguments.length();

	// Copy target address to mov eax, 0x42
	*reinterpret_cast<uintptr_t*>(&tailCode[1]) = reinterpret_cast<uintptr_t>(this->function);
	// Copy stack size to add esp, 0x00
	*reinterpret_cast<uint8_t*>(&tailCode[9]) = static_cast<uint8_t>(stackSize);

	uint8_t code[128];
	// Built the function
	{
		uint8_t *codePtr = &code[0];
		// Copy function header
		copyCode(&codePtr, headerCode, sizeof(headerCode));

		// Copy arguments
		for(int i = arguments.length() - 1; i >= 0; i--) {

			// Copy argument value to push 0x00
			*reinterpret_cast<int32_t*>(&pushCode[1]) = reinterpret_cast<int32_t>(arguments[i].integer);

			// Copy push code
			copyCode(&codePtr, pushCode, sizeof(pushCode));
		}

		// Copy function end
		copyCode(&codePtr, tailCode, sizeof(tailCode));
	}

	// Call some crazy
	uint32_t (*func)() = (uint32_t(*)())code;

	uint32_t result = func();

	return mkvar((Int)result);
}

extern "C" uint32_t __attribute__((cdecl)) cCodeFunction(int a, int b)
{
	kprintf("a=%d, b=%d\n", a, b);
	return 666;
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

	module->methods.add("afraid", new NativeMethod(reinterpret_cast<void*>(&cCodeFunction)));

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
