#include <stdlib.h>
#include <timer.h>
#include <dynamic.h>
#include <console.h>
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
	ker::Vector<Type> parameters;
public:
	NativeMethod(const char *arguments, void *fn) :
		function(fn)
	{
		Type *current = nullptr;
		while(*arguments) {
			switch(*arguments) {
				case '*':
					if(current == nullptr) die("NativeMethod.ctor.InvalidPointerSpec");
					current->pointer++;
					break;
				case 'i':
					current = &this->parameters.append(Type::Int);
					break;
				case 'r':
					current = &this->parameters.append(Type::Real);
					break;
				case 't':
					current = &this->parameters.append(Type::Text);
					break;
				default:
					die("NativeMethod.ctor.InvalidArgumentList");
			}
			arguments++;
		}
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
		return this->parameters;
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
	for(size_t i = 0; i < arguments.length(); i++) {
		if(arguments[i].type() != this->parameters[i]) {
			die_extra("NativeMethod.InvalidArgumentType", arguments[i].type().name());
		}
		// Special case TEXT: Copy const char * instead of object
		if(arguments[i].type() == Type::Text) {
			stackSize += sizeof(const char *);
		} else {
			stackSize += arguments[i].type().size();
		}
	}

	uint8_t *stack = (uint8_t*)malloc(stackSize);
	uint8_t *stackPtr = stack;
	for(size_t i = 0; i < arguments.length(); i++) {

		size_t size = arguments[i].type().size();
		void *data = arguments[i].data();

		if(arguments[i].type() == Type::Text) {
			// Copy const char *
			const char *text = arguments[i].value<Text>();

			memcpy(stackPtr, &text, sizeof(const char *));
			stackPtr += sizeof(const char *);
		}
		else {
			if(size > 0) {
				memcpy(stackPtr, data, size);
				stackPtr += size;
			}
		}
	}

	dynamic_call(this->function, stack, stackSize);

	free(stack);

	return Type::Int.createInstance();
}

extern "C" void __cdecl printInt(int i) {
	kprintf("{%d}\n", i);
}

extern "C" void __cdecl print2Int(int a, int b) {
	kprintf("{%d;%d}\n", a, b);
}

extern "C" void __cdecl printStr(const char *text) {
	kprintf("{%s}", text);
}

struct NativeModuleDef
{
	const char *name;
	const char *signature;
	void *function;
};

NativeModuleDef consoleModule[] = {
	{ "printInt", "i", (void*)printInt },
	{ "printStr", "t", (void*)printStr },
	{ "print2Int", "ii", (void*)print2Int },
	{ nullptr, nullptr, 0 }
};

static NativeModuleDef timerModule[] = {
	{ "sleep", "i", (void*)sleep },
	{ "timer_get", "", (void*)timer_get },
	{ "timer_set", "i", (void*)timer_set },
	{ nullptr, nullptr, 0 }
};

class KernelVM : public VM
{
public:
	Module *createNative(NativeModuleDef *mod)
	{
		Module *module = new Module();
		while(mod->name != nullptr) {
			module->methods.add(mod->name, new NativeMethod(mod->signature, mod->function));
			mod++;
		}
		return module;
	}

	Module *create(const ker::String &name) override
	{
		if(name == "/sys/timer") {
			return createNative(timerModule);
		}
		if(name == "/sys/console") {
			return createNative(consoleModule);
		}
		return nullptr;
	}
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

	KernelVM vm;

	Module *module = vm.load(mainfile.ptr, mainfile.size);
	if(module == nullptr) {
		kprintf("Could not load module :(\n");
		return;
	}

	kprintf("Module successfully loaded :)\n");

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
