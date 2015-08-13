#pragma once

extern "C" {
#include <stdlib.h>
#include <console.h>
}

// #include <map>

#include <ker/string.hpp>
#include <ker/vector.hpp>
#include <ker/dictionary.hpp>

// #include <vector>
// #include <string.h>

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
				case TypeID::Int: kprintf("%d", this->integer); break;
				case TypeID::Real: kprintf("%f", this->real); break;
				case TypeID::Bool: kprintf("%s", this->boolean ? "TRUE" : "FALSE"); break;
				default: kprintf("???"); break;
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

	class LocalContext  :
			public ker::Dictionary<ker::String, Variable*>
	{
	public:
		Module * const module = nullptr;

		LocalContext(Module *mod) :
			ker::Dictionary<ker::String, Variable*>(),
			module(mod)
		{

		}

		Variable *get(const ker::String &name)
		{
			if(this->contains(name)) {
				return this->at(name);
			} else {
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
		ker::Vector<Instruction*> instructions;

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
		ker::Vector<ker::Pair<ker::String, Variable>> arguments;
		ker::Dictionary<ker::String, Variable> locals;
		ker::Pair<ker::String, Variable> returnValue;

		Method(Module *module, Instruction *block) : module(module), block(block)
		{

		}

		Variable invoke(ker::Vector<Variable> arguments);
	};

	class Module
	{
	public:
		ker::Dictionary<ker::String, Variable*> variables;
		ker::Dictionary<ker::String, Method*> methods;
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
			return this->value;
		}
	};

	class VariableExpression :
			public Instruction
	{
	public:
		ker::String variableName;
		VariableExpression(ker::String variableName) : variableName(variableName) { }

		Variable execute(LocalContext &context) const override {
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
		ker::String variableName;
		Instruction *expression;
		VariableAssignmentExpression(ker::String variableName, Instruction *expression) :
			variableName(variableName),
			expression(expression)
		{

		}

		Variable execute(LocalContext &context) const override {
			if(this->expression == nullptr) {
				return Variable::Invalid;
			}
			Variable result = this->expression->execute(context);

			Variable *target = context.get(this->variableName);
			if(target == nullptr) {
				return Variable::Invalid;
			}

			if(target->type != result.type) {
				return Variable::Invalid;
			}

			switch(target->type.id) {
				case TypeID::Int: target->integer = result.integer; break;
				case TypeID::Real: target->real = result.real; break;
				case TypeID::Bool: target->boolean = result.boolean; break;
				default: break;
			}

			return result;
		}
	};

	class MethodInvokeExpression :
			public Instruction
	{
	public:
		ker::String methodName;
		ker::Vector<Instruction*> parameters;

		MethodInvokeExpression(ker::String methodName) :
			methodName(methodName)
		{

		}

		Variable execute(LocalContext &context) const override
		{
			Method *method = context.module->method(this->methodName.str());
			if(method == nullptr) {
				return Variable::Invalid;
			}

			ker::Vector<Variable> vars(this->parameters.length());
			vars.resize(this->parameters.length());
			for(size_t i =  0; i < vars.length(); i++) {
				vars[i] = this->parameters.at(i)->execute(context);
			}

			return method->invoke(vars);
		}
	};

	template<Variable (*OP)(Variable, Variable)>
	class ArithmeticExpression :
			public Instruction
	{
	public:
		Instruction *lhs, *rhs;

		ArithmeticExpression(Instruction *lhs, Instruction *rhs) :
			lhs(lhs),
			rhs(rhs)
		{

		}

		Variable execute(LocalContext &context) const override {
			if(this->lhs == nullptr) {
				return Variable::Invalid;
			}
			if(this->rhs == nullptr) {
				return Variable::Invalid;
			}

			Variable left = this->lhs->execute(context);
			Variable right = this->rhs->execute(context);

			if(left.type != right.type) {
				return Variable::Invalid;
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
				return Variable::Invalid;
			}

			Variable result = this->condition->execute(context);
			if(result.type != Type::Boolean) {
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

	class RepeatEndlessExpression :
			public Instruction
	{
	public:
		Instruction *block;

		RepeatEndlessExpression(Instruction *block) :
			block(block)
		{

		}

		Variable execute(LocalContext &context) const override {
			if(this->block == nullptr) {
				return Variable::Invalid;
			}

			while(true)
			{
				Variable result = this->block->execute(context);
				(void)result;
			}
			return Variable::Void;
		}
	};


	class RepeatWhileExpression :
			public Instruction
	{
	public:
		Instruction *condition;
		Instruction *block;

		RepeatWhileExpression(Instruction *condition, Instruction *block) :
			condition(condition),
			block(block)
		{

		}

		Variable execute(LocalContext &context) const override {
			if(this->condition == nullptr) {
				return Variable::Invalid;
			}
			if(this->block == nullptr) {
				return Variable::Invalid;
			}

			while(true)
			{
				Variable cond = this->condition->execute(context);
				if(cond.type != Type::Boolean) {
					return Variable::Invalid;
				}
				if(cond.boolean == false) {
					break;
				}

				this->block->execute(context);
			}
			return Variable::Void;
		}
	};
}
