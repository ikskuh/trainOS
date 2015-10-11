#pragma once

#include <vmvalue.hpp>

namespace csl
{
	ExceptionCode outb(VMValue &, const VMArray &args);

	ExceptionCode inb(VMValue &result, const VMArray &args);
}
