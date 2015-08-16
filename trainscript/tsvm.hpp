#pragma once

extern "C" {
#include <stdlib.h>
#include <console.h>
}

#include <ker/string.hpp>
#include <ker/vector.hpp>
#include <ker/dictionary.hpp>

#include "typeid.hpp"

namespace trainscript
{
	using Int = int32_t;
	using Real = float;
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

		const char *name() const {
			switch(id) {
				case TypeID::Invalid: return "INVALID";
				case TypeID::Void: return "VOID";
				case TypeID::Int: return "INT";
				case TypeID::Real: return "REAL";
				case TypeID::Text: return "TEXT";
				case TypeID::Bool: return "BOOL";
				default: return "unknown";
			}
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

		virtual Type expectedResult(LocalContext &) const {
			return Type::Void;
		}

		virtual bool validate(LocalContext &, ker::String &errorCode) const {
			errorCode = "";
			return true;
		}
	};

	class Block :
			public Instruction
	{
	public:
		ker::Vector<Instruction*> instructions;

		~Block() {
			for(auto *instr : instructions) delete instr;
		}

		bool validate(LocalContext &context, ker::String &errorCode) const {
			errorCode = "";
			for(auto *instr : instructions) {
				if(instr->validate(context, errorCode) == false)
					return false;
			}
			return true;
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
		virtual ~Method() { }

		virtual Variable invoke(ker::Vector<Variable> arguments) = 0;

		virtual bool validate(ker::String &errorCode) const = 0;

		virtual ker::Vector<Type> arguments() const = 0;

		virtual Type returnType() const = 0;
    };

    class ScriptMethod :
            public Method
	{
	public:
		Module *module;
		Instruction *block;
		bool isPublic;
		ker::Vector<ker::Pair<ker::String, Type>> mArguments;
		ker::Dictionary<ker::String, Type> mLocals;
		ker::Pair<ker::String, Type> mReturnValue;

        ScriptMethod(Module *module, Instruction *block) : module(module), block(block)
		{

		}

        Variable invoke(ker::Vector<Variable> arguments) override;

		bool validate(ker::String &errorCode) const override;

		ker::Vector<Type> arguments() const override
		{
			ker::Vector<Type> args(this->mArguments.length());
			for(size_t i = 0; i < args.length(); i++) {
				args[i] = this->mArguments[i].second;
			}
			return args;
		}

		Type returnType() const override
		{
			return this->mReturnValue.second;
		}
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
            return this->methods.get(name);
		}

		Variable *variable(const char *name)
		{
            return this->variables.get(name);
		}

		bool validate(ker::String &errorCode) const;
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

		Variable execute(LocalContext &) const override {
			return this->value;
		}

		Type expectedResult(LocalContext &) const override {
			return this->value.type;
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
				die_extra("VariableExpression.VariableNotFound", this->variableName.str());
			}
			return *var;
		}

		bool validate(LocalContext &context, ker::String &errorCode) const override {
			errorCode = "";
			if(context.get(this->variableName) == nullptr) {
				errorCode = "Variable " + this->variableName + " not found.";
				return false;
			}
			return true;
		}

		Type expectedResult(LocalContext &context) const override {
			Variable *var = context.get(this->variableName);
			if(var == nullptr) {
				return Type::Invalid;
			} else {
				return var->type;
			}
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
				die("VariableAssignmentExpression.ExpressionMissing");
			}
			Variable result = this->expression->execute(context);

			Variable *target = context.get(this->variableName);
			if(target == nullptr) {
				die_extra("VariableAssignmentExpression.VariableNotFound", this->variableName.str());
			}

			if(target->type != result.type) {
				die_extra("VariableAssignmentExpression.ExpectedType", result.type.name());
			}

			switch(target->type.id) {
				case TypeID::Int: target->integer = result.integer; break;
				case TypeID::Real: target->real = result.real; break;
				case TypeID::Bool: target->boolean = result.boolean; break;
				default: break;
			}

			return result;
		}

		bool validate(LocalContext &context, ker::String &errorCode) const override {
			errorCode = "";
			if(this->expression == nullptr) {
				errorCode = "Missing expression.";
				return false;
			}
			Type result = this->expression->expectedResult(context);
			if(result == Type::Invalid) {
				errorCode = "Expression returns invalid type.";
				return false;
			}
			if(result == Type::Void) {
				errorCode = "Void cannot be assigned to a variable";
				return false;
			}
			Variable *var = context.get(this->variableName);
			if(var == nullptr) {
				errorCode = "Variable " + this->variableName + " not found.";
				return false;
			}
			if(var->type != result) {
				errorCode = "Variable assignment has invalid type.";
				return false;
			}
			if(this->expression->validate(context, errorCode) == false)
				return false;
			return true;
		}

		Type expectedResult(LocalContext &context) const override {
			if(this->expression == nullptr) {
				return Type::Invalid;
			} else {
				return this->expression->expectedResult(context);
			}
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
				die_extra("MethodInvokeExpression.MethodNotFound", this->methodName.str());
			}

			ker::Vector<Variable> vars(this->parameters.length());
			vars.resize(this->parameters.length());
			for(size_t i =  0; i < vars.length(); i++) {
				vars[i] = this->parameters.at(i)->execute(context);
			}

			return method->invoke(vars);
		}

		bool validate(LocalContext &context, ker::String &errorCode) const override {
			Method *method = context.module->method(this->methodName.str());
			if(method == nullptr) {
				errorCode = "The method " + this->methodName + " does not exist.";
				return false;
			}

			ker::Vector<Type> arguments = method->arguments();
			if(arguments.length() != this->parameters.length()) {
				errorCode = "Argument count mismatch.";
				return false;
			}
			for(size_t i = 0; i < arguments.length(); i++) {
				if(this->parameters[i]->expectedResult(context) != arguments[i]) {
					errorCode = "Argument type mismatch.";
					return false;
				}
				if(this->parameters[i]->validate(context, errorCode) == false)
					return false;
			}

			return true;
		}

		Type expectedResult(LocalContext &context) const override {
			Method *method = context.module->method(this->methodName.str());
			if(method == nullptr) {
				return Type::Invalid;
			}
			return method->returnType();
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
				die_extra("ArithmeticExpression.ExpressionMissing", "Left-hand side");
			}
			if(this->rhs == nullptr) {
				die_extra("ArithmeticExpression.ExpressionMissing", "Right-hand side");
			}

			Variable left = this->lhs->execute(context);
			Variable right = this->rhs->execute(context);

			if(left.type != right.type) {
				die("ArithmeticExpression.TypeMismatch");
			}

			Variable result = OP(left, right);

			if(result.type.usable() == false) {
				die_extra("ArithmeticExpression.InvalidResult", result.type.name());
			}
			return result;
		}

		bool validate(LocalContext &context, ker::String &errorCode) const override {
			errorCode = "";
			if(this->lhs == nullptr) {
				errorCode = "Left part of operand is missing.";
				return false;
			}
			if(this->rhs == nullptr) {
				errorCode = "Right part of operand is missing.";
				return false;
			}
			Type lhsType = this->lhs->expectedResult(context);
			Type rhsType = this->rhs->expectedResult(context);
			if(lhsType != rhsType) {
				errorCode = "Types of operands do not match.";
				return false;
			}
			if(lhsType == Type::Invalid) {
				errorCode = "Invalid type can't be used for operand.";
				return false;
			}
			if(rhsType == Type::Void) {
				errorCode = "VOID type can't be used for operand.";
				return false;
			}

			if(this->lhs->validate(context, errorCode) == false)
				return false;
			if(this->rhs->validate(context, errorCode) == false)
				return false;

			return true;
		}

		Type expectedResult(LocalContext &context) const override {
			if(this->lhs == nullptr) {
				return Type::Invalid;
			} else {
				return this->lhs->expectedResult(context);
			}
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
				die("IfExpression.ConditionMissing");
			}

			Variable result = this->condition->execute(context);
			if(result.type != Type::Boolean) {
				die_extra("IfExpression.TypeMismatch", result.type.name());
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
				die("RepeatEndlessExpression.BlockMissing");
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
				die("RepeatWhileExpression.ConditionMissing");
			}
			if(this->block == nullptr) {
				die("RepeatWhileExpression.BlockMissing");
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
