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
	using Bool = bool;

	extern bool verbose;

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
			return (this->id != TypeID::Invalid) &&
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

		static const Type Invalid;
		static const Type Void;
		static const Type Int;
		static const Type Real;
		static const Type Text;
		static const Type Boolean;
	};

	struct Variable
	{
		Type type;
		union {
			Int integer;
			Real real;
			Text text;
			Bool boolean;
		};

		void printval() const
		{
			switch(this->type.id) {
				case TypeID::Int: printf("%d", this->integer); break;
				case TypeID::Real: printf("%f", this->real); break;
				case TypeID::Bool: printf("%s", this->boolean ? "TRUE" : "FALSE"); break;
				default: printf("???"); break;
			}
		}

		static const Variable Invalid;
		static const Variable Void;
		static const Variable Int;
		static const Variable Real;
		static const Variable Text;
		static const Variable Boolean;
	};

	static inline Variable mkvar(Int value) {
		Variable v = Variable::Int;
		v.integer = value;
		return v;
	}

	static inline Variable mkvar(Real value) {
		Variable v = Variable::Real;
		v.real = value;
		return v;
	}

	static inline Variable mkbool(Bool value) {
		Variable v = Variable::Boolean;
		v.boolean = value;
		return v;
	}

	class Module;

	class LocalContext :
			public std::map<std::string, Variable*>
	{
	public:
		Module * const module = nullptr;

		LocalContext(Module *mod) :
			std::map<std::string, Variable*>(),
			module(mod)
		{

		}

		int depth;

		LocalContext() : depth(0) { }

		void indent() {
			for(int i = 0; i < depth; i++) printf("  ");
		}

		Variable *get(const std::string &name)
		{
			if(this->count(name) > 0) {
				return this->at(name);
			} else {
				printf("Variable %s not found!\n", name.c_str());
				return nullptr;
			}
		}
	};

	class Instruction
	{
	public:
		virtual ~Instruction() { }

		virtual Variable execute(LocalContext &context) const = 0;
	};

	class Block :
			public Instruction
	{
	public:
		std::vector<Instruction*> instructions;

		~Block() {
			for(auto *instr : instructions) delete instr;
		}

		Variable execute(LocalContext &context) const override {
			for(auto *instr : instructions) {
				instr->execute(context);
			}
			return Variable::Void;
		}
	};

	class Method
	{
	public:
		Module *module;
		Instruction *block;
		bool isPublic;
		std::vector<std::pair<std::string, Variable>> arguments;
		std::map<std::string, Variable> locals;
		std::pair<std::string, Variable> returnValue;

		Method(Module *module, Instruction *block) : module(module), block(block)
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
		ConstantExpression(Variable value) : value(value) { }

		Variable execute(LocalContext &context) const override {
			if(verbose) {
				context.indent();
				printf("constant: ");
				this->value.printval();
				printf("\n");
			}
			return this->value;
		}
	};

	class VariableExpression :
			public Instruction
	{
	public:
		std::string variableName;
		VariableExpression(std::string variableName) : variableName(variableName) { }

		Variable execute(LocalContext &context) const override {
			if(verbose) {
				context.indent();
				printf("variable: %s\n", this->variableName.c_str());
			}

			auto *var = context.get(this->variableName);
			if(var == nullptr) {
				return Variable::Invalid;
			}
			return *var;
		}
	};


	class VariableAssignmentExpression :
			public Instruction
	{
	public:
		std::string variableName;
		Instruction *expression;
		VariableAssignmentExpression(std::string variableName, Instruction *expression) :
			variableName(variableName),
			expression(expression)
		{

		}

		Variable execute(LocalContext &context) const override {
			if(this->expression == nullptr) {
				if(verbose) printf("Invalid instruction in assignment.\n");
				return Variable::Invalid;
			}
			if(verbose) context.depth++;
			Variable result = this->expression->execute(context);
			if(verbose) context.depth--;

			if(verbose) {
				context.indent();
				printf("assign ");
				result.printval();
				printf(" to %s\n", this->variableName.c_str());
			}

			Variable *target = context.get(this->variableName);
			if(target == nullptr) {
				return Variable::Invalid;
			}

			if(target->type != result.type) {
				if(verbose) printf(
					"Assignment does not match: %s → %s\n",
					typeName(result.type.id),
					this->variableName.c_str());
				return Variable::Invalid;
			}

			switch(target->type.id) {
				case TypeID::Int: target->integer = result.integer; break;
				case TypeID::Real: target->real = result.real; break;
				case TypeID::Bool: target->boolean = result.boolean; break;
				default: if(verbose) printf("assignment not supported.\n"); break;
			}

			return result;
		}
	};

	class MethodInvokeExpression :
			public Instruction
	{
	public:
		std::string methodName;
		std::vector<Instruction*> parameters;

		MethodInvokeExpression(std::string methodName) :
			methodName(methodName)
		{

		}

		Variable execute(LocalContext &context) const override
		{
			Method *method = context.module->method(this->methodName.c_str());
			if(method == nullptr) {
				if(verbose) printf("method %s not found!\n", this->methodName.c_str());
				return Variable::Invalid;
			}

			if(verbose) context.depth++;
			std::vector<Variable> vars(this->parameters.size());
			for(int i =  0; i < vars.size(); i++) {
				vars[i] = this->parameters.at(i)->execute(context);
			}
			if(verbose) context.depth--;

			return method->invoke(vars);
		}
	};

	template<Variable (*OP)(Variable, Variable)>
	class ArithmeticExpression :
			public Instruction
	{
	public:
		Instruction *rhs, *lhs;

		ArithmeticExpression(Instruction *lhs, Instruction *rhs) :
			lhs(lhs),
			rhs(rhs)
		{

		}

		Variable execute(LocalContext &context) const override {
			if(this->lhs == nullptr) {
				if(verbose) printf("lhs: Invalid instruction in addition.\n");
				return Variable::Invalid;
			}
			if(this->rhs == nullptr) {
				if(verbose) printf("rhs: Invalid instruction in addition.\n");
				return Variable::Invalid;
			}

			if(verbose) context.depth++;
			Variable left = this->lhs->execute(context);
			Variable right = this->rhs->execute(context);
			if(verbose) context.depth--;

			if(left.type != right.type) {
				if(verbose) printf(
					"Arithmetic types do not match: %s != %s\n",
					typeName(left.type.id),
					typeName(right.type.id));
				return Variable::Invalid;
			}

			if(verbose) {
				context.indent();
				printf("Arithmetic on ");
				left.printval();
				printf(" and ");
				right.printval();
				printf("\n");
			}

			return OP(left, right);
		}
	};

	class IfExpression :
			public Instruction
	{
	public:
		Instruction *condition;
		Instruction *blockTrue;
		Instruction *blockFalse;

		IfExpression(Instruction *condition, Instruction *blockTrue, Instruction *blockFalse) :
			condition(condition),
			blockTrue(blockTrue),
			blockFalse(blockFalse)
		{

		}

		Variable execute(LocalContext &context) const override {
			if(this->condition == nullptr) {
				if(verbose) printf("IF: missing condition.\n");
				return Variable::Invalid;
			}

			Variable result = this->condition->execute(context);
			if(result.type != Type::Boolean) {
				if(verbose) printf("IF: Invalid condition type.\n");
				return Variable::Invalid;
			}
			if((result.boolean == true) && (this->blockTrue != nullptr)) {
				this->blockTrue->execute(context);
			}
			if((result.boolean == false) && (this->blockFalse != nullptr)) {
				this->blockFalse->execute(context);
			}
			return Variable::Void;
		}
	};
}
