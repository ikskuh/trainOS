#pragma once
#include <stdlib.h>
#include "module.hpp"

#include "method.hpp"
#include "instruction.hpp"

namespace trainscript
{
	class Block :
			public Instruction
	{
	public:
		ker::Vector<Instruction*> instructions;

		~Block() {
			for(auto *instr : instructions) delete instr;
		}

		bool validate(ExecutionContext &context, ker::String &errorCode) const {
			errorCode = "";
			for(auto *instr : instructions) {
				if(instr->validate(context, errorCode) == false)
					return false;
			}
			return true;
		}

		Variable execute(ExecutionContext &context) const override {
			for(auto *instr : instructions) {
				instr->execute(context);
			}
			return Variable::Void;
		}
	};

	class ConstantExpression :
			public Instruction
	{
	public:
		Variable value;
		ConstantExpression(const Variable &value) : value(value) { }

		Variable execute(ExecutionContext &) const override {
			return this->value;
		}

		Type expectedResult(ExecutionContext &) const override {
			return this->value.type();
		}
	};

	class VariableExpression :
			public Instruction
	{
	public:
		ker::String variableName;
		VariableExpression(ker::String variableName) : variableName(variableName) { }

		Variable execute(ExecutionContext &context) const override {
			auto *var = context.get(this->variableName);
			if(var == nullptr) {
				die_extra("VariableExpression.VariableNotFound", this->variableName.str());
			}
			return *var;
		}

		bool validate(ExecutionContext &context, ker::String &errorCode) const override {
			errorCode = "";
			if(context.get(this->variableName) == nullptr) {
				errorCode = "Variable " + this->variableName + " not found.";
				return false;
			}
			return true;
		}

		Type expectedResult(ExecutionContext &context) const override {
			Variable *var = context.get(this->variableName);
			if(var == nullptr) {
				return Type::Invalid;
			} else {
				return var->type();
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

		Variable execute(ExecutionContext &context) const override {
			if(this->expression == nullptr) {
				die("VariableAssignmentExpression.ExpressionMissing");
			}
			Variable result = this->expression->execute(context);

			Variable *target = context.get(this->variableName);
			if(target == nullptr) {
				die_extra("VariableAssignmentExpression.VariableNotFound", this->variableName.str());
			}

			if(target->type() != result.type()) {
				die_extra("VariableAssignmentExpression.ExpectedType", result.type().name());
			}

			*target = result;

			return *target;
		}

		bool validate(ExecutionContext &context, ker::String &errorCode) const override {
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
			if(var->type() != result) {
				errorCode = "Variable assignment has invalid type.";
				return false;
			}
			if(this->expression->validate(context, errorCode) == false)
				return false;
			return true;
		}

		Type expectedResult(ExecutionContext &context) const override {
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
		ker::String moduleName;
		ker::String methodName;
		ker::Vector<Instruction*> parameters;

		MethodInvokeExpression(const ker::String moduleName, const ker::String &methodName) :
			moduleName(moduleName),
			methodName(methodName)
		{

		}

		Variable execute(ExecutionContext &context) const override
		{
			Module *module = nullptr;
			if((this->moduleName.length() == 0) || (this->moduleName == ker::String("this"))) {
				module = context.module;
			} else {
				module = context.module->object(this->moduleName);
			}
			if(module == nullptr) {
				die_extra("MethodInvokeExpression.InvalidModule", this->moduleName.str());
			}

			Method *method = module->method(this->methodName.str());
			if(method == nullptr) {
				die_extra("MethodInvokeExpression.MethodNotFound", this->methodName.str());
			}

			ker::Vector<Variable> vars(this->parameters.length());
			vars.resize(this->parameters.length());
			for(size_t i =  0; i < vars.length(); i++) {
				vars[i].replace(this->parameters.at(i)->execute(context));
			}

			return method->invoke(vars);
		}

		bool validate(ExecutionContext &context, ker::String &errorCode) const override {

			Module *module = nullptr;
			if((this->moduleName.length() == 0) || (this->moduleName == ker::String("this"))) {
				module = context.module;
			} else {
				module = context.module->object(this->moduleName);
			}

			Method *method = module->method(this->methodName.str());
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

		Type expectedResult(ExecutionContext &context) const override {
			Method *method = context.module->method(this->methodName.str());
			if(method == nullptr) {
				return Type::Invalid;
			}
			return method->returnType();
		}
	};

	class ArithmeticExpression :
			public Instruction
	{
	public:
		Operation op;
		Instruction *lhs, *rhs;

		ArithmeticExpression(Instruction *lhs, Instruction *rhs, Operation op) :
			op(op),
			lhs(lhs),
			rhs(rhs)
		{

		}

		Variable execute(ExecutionContext &context) const override {
			if(this->lhs == nullptr) {
				die_extra("ArithmeticExpression.ExpressionMissing", "Left-hand side");
			}
			if(this->rhs == nullptr) {
				die_extra("ArithmeticExpression.ExpressionMissing", "Right-hand side");
			}

			Variable left = this->lhs->execute(context);
			Variable right = this->rhs->execute(context);

			if(left.type() != right.type()) {
				die("ArithmeticExpression.TypeMismatch");
			}

			if(left.type().hasOperator(this->op) == false) {
				die_extra("ArithmeticExpression.InvalidOperator", "The operator was not defined for this type.");
			}

			Variable result = left.type().apply(left, this->op, right);

			if(result.type().usable() == false) {
				die_extra("ArithmeticExpression.InvalidResult", result.type().name());
			}
			return result;
		}

		bool validate(ExecutionContext &context, ker::String &errorCode) const override {
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

			if(lhsType.hasOperator(this->op) == false) {
				errorCode = "The operator is not defined for this type.";
				return false;
			}

			if(this->lhs->validate(context, errorCode) == false)
				return false;
			if(this->rhs->validate(context, errorCode) == false)
				return false;

			return true;
		}

		Type expectedResult(ExecutionContext &context) const override {
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

		Variable execute(ExecutionContext &context) const override {
			if(this->condition == nullptr) {
				die("IfExpression.ConditionMissing");
			}

			Variable result = this->condition->execute(context);
			if(result.type() != Type::Bool) {
				die_extra("IfExpression.TypeMismatch", result.type().name());
			}
			if((result.value<Bool>() == true) && (this->blockTrue != nullptr)) {
				this->blockTrue->execute(context);
			}
			if((result.value<Bool>() == false) && (this->blockFalse != nullptr)) {
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

		Variable execute(ExecutionContext &context) const override {
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

		Variable execute(ExecutionContext &context) const override {
			if(this->condition == nullptr) {
				die("RepeatWhileExpression.ConditionMissing");
			}
			if(this->block == nullptr) {
				die("RepeatWhileExpression.BlockMissing");
			}

			while(true)
			{
				Variable cond = this->condition->execute(context);
				if(cond.type() != Type::Bool) {
					return Variable::Invalid;
				}
				if(cond.value<Bool>() == false) {
					break;
				}

				this->block->execute(context);
			}
			return Variable::Void;
		}
	};
}
