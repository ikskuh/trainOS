#pragma once

#include <vmtype.hpp>

#include <cpustate.h>

namespace csl
{
	extern const VMType * CpuStateType;

	VMValue createCpuState(::CpuState *state);
}
