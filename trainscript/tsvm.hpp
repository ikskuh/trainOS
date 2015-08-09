#pragma once

#include <map>
#include <string>
#include <vector>
#include <string.h>

#include "typeid.hpp"

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

	struct Type
	{
		TypeID id;
		int pointer;

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

		void printval() const
		{
			switch(this->type.id) {
				case TypeID::Int: printf("%d", this->integer); break;
				case TypeID::Real: printf("%f", this->real); break;
				default: printf("???"); break;
			}
		}
	};

	static inline Variable mkvar(Type type) { return { type, 0 }; }

	static inline Variable mkvar(TypeID type) { return { { type, 0 }, 0 }; }

	static inline Variable mkvar(Int value) { return { { TypeID::Int, 0 }, value }; }

	class Module;

	class LocalContext :
			public std::map<std::string, Variable*>
	{
	public:
		int depth;

		LocalContext() : depth(0) { }

		void indent() {
			for(int i = 0; i < depth; i++) printf("  ");
		}
	};

	class Instruction
	{
	protected:
		Module *module;
	public:
		Instruction (Module *module) : module(module)
		{

		}

		virtual ~Instruction() { }

		virtual Variable execute(LocalContext &context) const = 0;
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

		Variable execute(LocalContext &context) const override {
			for(auto *instr : instructions) {
				instr->execute(context);
			}
			return mkvar(TypeID::Void);
		}
	};

	class Method
	{
	public:
		Module *module;
		Block *block;
		bool isPublic;
		std::vector<std::pair<std::string, Variable>> arguments;
		std::map<std::string, Variable> locals;
		std::pair<std::string, Variable> returnValue;

		Method(Module *module, Block *block) : module(module), block(block)
		{

		}

		Variable invoke(std::vector<Variable> arguments);
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





	// Instructions

	class ConstantExpression :
			public Instruction
	{
	public:
		Variable value;
		ConstantExpression(Module *mod, Variable value) : Instruction(mod), value(value) { }

		Variable execute(LocalContext &context) const override {
			context.indent(); printf("constant: "); this->value.printval(); printf("\n");
			return this->value;
		}
	};

	class VariableExpression :
			public Instruction
	{
	public:
		std::string variableName;
		VariableExpression(Module *mod, std::string variableName) : Instruction(mod), variableName(variableName) { }

		Variable execute(LocalContext &context) const override {
			context.indent(); printf("variable: %s\n", this->variableName.c_str());
			if(context.count(this->variableName) > 0) {
				return *context.at(this->variableName);
			} else {
				auto *var = this->module->variable(this->variableName.c_str());
				if(var != nullptr) {
					return *var;
				} else {
					printf("Variable not found: %s\n", this->variableName.c_str());
					return mkvar(TypeID::Void);
				}
			}
		}
	};


	class VariableAssignmentExpression :
			public Instruction
	{
	public:
		std::string variableName;
		Instruction *expression;
		VariableAssignmentExpression(Module *mod, std::string variableName, Instruction *expression) :
			Instruction(mod),
			variableName(variableName),
			expression(expression)
		{

		}

		Variable execute(LocalContext &context) const override {
			if(this->expression == nullptr) {
				printf("Invalid instruction in assignment.\n");
				return mkvar(TypeID::Void);
			}
			context.depth++;
			Variable result = this->expression->execute(context);
			context.depth--;

			context.indent(); printf("assign "); result.printval(); printf(" to %s\n", this->variableName.c_str());

			Variable *target = nullptr;
			if(context.count(this->variableName) > 0) {
				target = context.at(this->variableName);
			} else {
				target = this->module->variable(this->variableName.c_str());
			}

			if(target == nullptr) {
				printf("Variable not found: %s\n", this->variableName.c_str());
				return mkvar(TypeID::Void);
			}

			if(target->type != result.type) {
				printf(
					"Assignment does not match: %s → %s\n",
					typeName(result.type.id),
					this->variableName.c_str());
				return mkvar(TypeID::Void);
			}

			switch(target->type.id) {
				case TypeID::Int: target->integer = result.integer; break;
				case TypeID::Real: target->real = result.real; break;
				default: printf("assignment not supported.\n"); break;
			}


			context.indent();
			context.at("a")->printval();
			printf(" ");
			context.at("b")->printval();
			printf("\n");

			return result;
		}
	};

	template<Variable (*OP)(Variable, Variable)>
	class ArithmeticExpression :
			public Instruction
	{
	public:
		Instruction *rhs, *lhs;

		ArithmeticExpression(Module *mod, Instruction *lhs, Instruction *rhs) :
			Instruction(mod),
			lhs(lhs),
			rhs(rhs)
		{

		}

		Variable execute(LocalContext &context) const override {
			if(this->lhs == nullptr) {
				printf("lhs: Invalid instruction in addition.\n");
				return mkvar(TypeID::Void);
			}
			if(this->rhs == nullptr) {
				printf("rhs: Invalid instruction in addition.\n");
				return mkvar(TypeID::Void);
			}

			context.depth++;
			Variable left = this->lhs->execute(context);
			Variable right = this->rhs->execute(context);
			context.depth--;

			if(left.type != right.type) {
				printf(
					"Arithmetic types do not match: %s != %s\n",
					typeName(left.type.id),
					typeName(right.type.id));
				return mkvar(TypeID::Void);
			}

			context.indent();
			printf("Arithmetic on ");
			left.printval();
			printf(" and ");
			right.printval();
			printf("\n");

			return OP(left, right);
		}
	};
}
