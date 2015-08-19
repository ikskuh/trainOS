#pragma once

#include "module.hpp"
#include "method.hpp"

namespace trainscript
{
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
}
