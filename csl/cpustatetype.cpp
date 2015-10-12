#include "cpustatetype.hpp"

#include <types/compoundtype.hpp>

#include <console.h>

CompoundType type {
	"CPUSTATE",
	{
		{ "eax", &VMType::UInt32 },
		{ "ebx", &VMType::UInt32 },
		{ "ecx", &VMType::UInt32 },
		{ "edx", &VMType::UInt32 },
		{ "esi", &VMType::UInt32 },
		{ "edi", &VMType::UInt32 },
		{ "ebp", &VMType::UInt32 },

		{ "intr", &VMType::UInt32 },
		{ "error", &VMType::UInt32 },

		{ "eip", &VMType::UInt32 },
		{ "cs", &VMType::UInt32 },
		{ "eflags", &VMType::UInt32 },
		{ "esp", &VMType::UInt32 },
		{ "ss", &VMType::UInt32 },
	}
};

namespace csl
{
	const VMType * CpuStateType = &type;

	VMValue createCpuState(CpuState *state)
	{
		Compound object = new ker::Dictionary<VMText, VMValue>();

#define COPY(name) (*object)[#name] = VMValue::UInt32(state->name)
		COPY(eax);
		COPY(ebx);
		COPY(ecx);
		COPY(edx);
		COPY(esi);
		COPY(edi);
		COPY(ebp);

		COPY(intr);
		COPY(error);

		COPY(eip);
		COPY(cs);
		COPY(eflags);
		COPY(esp);
		COPY(ss);
#undef COPY

		return VMValue(CpuStateType, &object);

	}
}
