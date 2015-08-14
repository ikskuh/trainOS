#include <stdlib.h>
#include <timer.h>
#include <dynamic.h>
#include "../trainscript/tsvm.hpp"

using namespace ker;
using namespace trainscript;

extern "C" {
	extern const char mainscript_start;
	extern const char mainscript_end;
	extern const char mainscript_size;
}

void cpp_test();

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

	bool validate(ker::String &errorCode) const
	{
		if(this->function == nullptr) {
			errorCode = "Native method with nullptr interface.";
			return false;
		}
		return true;
	}

	Vector<Type> arguments() const {
		Vector<Type> list;
		list.append(Type::Int);
		return list;
	}

	Type returnType() const {
		return Type::Int;
	}
};

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

	// Copy arguments
	size_t stackSize = 0;
	for(int i = 0; i < arguments.length(); i++) {
		switch(arguments[i].type.id) {
			case TypeID::Bool: stackSize += sizeof(Int); break;
			case TypeID::Int:  stackSize += sizeof(Int); break;
			case TypeID::Real: stackSize += sizeof(Real); break;
			default: die("invalid argument type."); break;
		}
	}

	uint8_t *stack = (uint8_t*)malloc(stackSize);
	for(int i = arguments.length() - 1; i >= 0; i--) {
		switch(arguments[i].type.id) {
			case TypeID::Bool:
				*reinterpret_cast<Int*>(stack) = arguments[i].boolean ? 1 : 0;
				stack += sizeof(Int);
				break;
			case TypeID::Int:
				*reinterpret_cast<Int*>(stack) = arguments[i].integer;
				stack += sizeof(Int);
				break;
			case TypeID::Real:
				*reinterpret_cast<Real*>(stack) = arguments[i].real;
				stack += sizeof(Real);
				break;
			default: die("invalid argument type."); break;
		}
	}

	dynamic_call(this->function, stack-stackSize, stackSize);

	free(stack);

	return mkvar((Int)0);
}

extern "C" void __cdecl printInt(int i) {
	kprintf("{%d}\n", i);
}

struct NativeModuleDef
{
	const char *name;
	void *function;
};

NativeModuleDef methods[] = {
	{ "sleep", (void*)sleep },
	{ "timer_get", (void*)timer_get },
	{ "timer_set", (void*)timer_set },
	{ "printInt", (void*)printInt },
	{ nullptr, 0 }
};

extern "C" void vm_start()
{
	struct {
		const char *ptr;
		uint32_t size;
	} mainfile {
		&mainscript_start,
		(uint32_t)&mainscript_size
	};

	// cpp_test();

	kprintf("Parse kernel module: ");
	Module *module = VM::load(mainfile.ptr, mainfile.size);
	if(module == nullptr) {
		kprintf("Could not load module :(\n");
		return;
	}

	kprintf("Module successfully loaded :)\n");

	// Load native modules
	NativeModuleDef *mod = methods;
	while(mod->name != nullptr) {
		module->methods.add(mod->name, new NativeMethod(mod->function));
		mod++;
	}

	String errorCode;
	if(module->validate(errorCode) == false) {
		kprintf("Module validation failed: \x12\x06%s\x12\x07\n", errorCode.str());
		return;
	}

	Method *main = module->method("main");
	if(main == nullptr) {
		kprintf("Script has no main method.\n");
		return;
	}

	main->invoke({ });

	delete module;
}


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
