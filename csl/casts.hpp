#pragma once

#include <vmvalue.hpp>

namespace csl
{
	ExceptionCode toInt8(VMValue &res, const VMArray &args);
	ExceptionCode toInt16(VMValue &res, const VMArray &args);
	ExceptionCode toInt32(VMValue &res, const VMArray &args);

	ExceptionCode toUInt8(VMValue &res, const VMArray &args);
	ExceptionCode toUInt16(VMValue &res, const VMArray &args);
	ExceptionCode toUInt32(VMValue &res, const VMArray &args);

	ExceptionCode toBool(VMValue &res, const VMArray &args);
	ExceptionCode toText(VMValue &res, const VMArray &args);
}
