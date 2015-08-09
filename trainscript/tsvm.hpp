#pragma once

#include <map>
#include <string>
#include <vector>

#include "common.h"

namespace trainscript
{
	using Int = int32_t;
	using Real = double;
	using Void = void;

	struct Text
	{
		size_t length;
		char *data;
	};

	enum class TypeID
	{
		Unknown = tidUNKNOWN,
		Void = tidVOID,
		Int = tidINT,
		Real = tidREAL,
		Text = tidTEXT
	};

	struct Type
	{
		TypeID id;
		int pointer;

		Type() : id(TypeID::Unknown), pointer(0) { }
		Type(TypeID id) : id(id), pointer(0) { }
		Type(TypeID id, int pointer) : id(id), pointer(pointer) { }

		Type reference() const {
			return { id, pointer + 1 };
		}

		Type dereference() const {
			if(pointer == 0) throw std::exception();
			return { id, pointer - 1 };
		}

		bool usable() const {
			return (this->id != TypeID::Unknown) &&
				((this->id != TypeID::Void) || (this->pointer > 0));
		}

		bool operator ==(const Type &other) const {
			return (this->id == other.id) &&
				   (this->pointer == other.pointer);
		}

		bool operator !=(const Type &other) const {
			return (this->id != other.id) ||
				   (this->pointer != other.pointer);
		}
	};

	struct Variable
	{
		Type type;
		union {
			Int integer;
			Real real;
			Text text;
		};

		Variable() : type(TypeID::Unknown), integer(0) { }

		explicit Variable(Type type) : type(type), integer(0) { }

		explicit Variable(TypeID type) : type(type), integer(0) { }

		explicit Variable(Int integer) : type(TypeID::Int), integer(integer) { }

		explicit Variable(Real real) : type(TypeID::Real), real(real) { }
	};

	class Module;

	using LocalContext = std::map<std::string, Variable&>;

	class Instruction
	{
		Module *module;
	public:
		Instruction (Module *module) : module(module)
		{

		}

		virtual ~Instruction() { }

		virtual void execute(LocalContext &context) const = 0;
	};

	class Block :
			public Instruction
	{
	public:
		std::vector<Instruction*> instructions;

		Block(Module *module) : Instruction(module) { }

		~Block() {
			for(auto *instr : instructions) delete instr;
		}

		void execute(LocalContext &context) const override {
			for(auto *instr : instructions) {
				instr->execute(context);
			}
		}
	};

	class DebugInstruction :
			public Instruction
	{
	public:
		std::string message;

		DebugInstruction(Module *module, std::string msg) : Instruction(module), message(msg) { }

		void execute(LocalContext &context) const override {
			printf("debug: %s\n", message.c_str());
		}
	};

	class DebugVariableInstruction :
			public Instruction
	{
	public:
		std::string variable;

		DebugVariableInstruction(Module *module, std::string variable) : Instruction(module), variable(variable) { }

		void execute(LocalContext &context) const override {
			if(context.count(variable) > 0) {
				auto &var = context.at(variable);
				switch(var.type.id) {
				case TypeID::Int:
					printf("%s := %d\n", variable.c_str(), var.integer);
					break;
				case TypeID::Real:
					printf("%s := %f\n", variable.c_str(), var.real);
					break;
				default:
					printf("%s has unknown type.\n", variable.c_str());
					break;
				}
			} else {
				printf("variable %s not found.\n", variable.c_str());
			}
		}
	};

	class Method
	{
	public:
		Module *module;
		Block *block;
		std::vector<std::pair<std::string, Variable>> arguments;
		std::map<std::string, Variable> locals;
		std::pair<std::string, Variable> returnValue;

		Method(Module *module, Block *block) : module(module), block(block)
		{

		}

		Variable invoke(std::vector<Variable> arguments)
		{
			LocalContext context;
			if(this->returnValue.second.type.usable()) {
				context.insert({ this->returnValue.first, this->returnValue.second });
			}
			if(arguments.size() != this->arguments.size()) {
				printf("MECKER anzahl!\n");
				return Variable();
			}
			for(size_t i = 0; i < this->arguments.size(); i++) {
				if(this->arguments[i].second.type != arguments[i].type) {
					printf("MECKER argtyp!\n");
					return Variable();
				}
				context.insert({this->arguments[i].first, arguments[i] });
			}
			for(auto local : this->locals) {
				context.insert({ local.first, local.second });
			}


			this->block->execute(context);

			return this->returnValue.second;
		}
	};

	class Module
	{
	public:
		std::map<std::string, Variable*> variables;
		std::map<std::string, Method*> methods;
	public:
		Module();
		~Module();

		Method *method(const char *name)
		{
			return this->methods[name];
		}

		Variable *variable(const char *name)
		{
			return this->variables[name];
		}
	};

	class VM
	{
	public:
		static Module *load(const void *buffer, size_t length);

		static Module *load(const char *text);
	};
}
