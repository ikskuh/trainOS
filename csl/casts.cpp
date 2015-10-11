#include "casts.hpp"

#include <vmtype.hpp>

namespace csl
{
#define CONVERT(_from, _to) if(source.type() == VMType::_from) { \
		res = VMValue::_to(source.value<VM##_from>()); \
		return ExceptionCode::None; \
	}
#define TOINT(targetType) ExceptionCode to##targetType(VMValue &res, const VMArray &args) \
{ \
	if(args.length() != 1) return ExceptionCode::InvalidArgument; \
	const VMValue &source = args[0]; \
	if(source.type() == VMType::targetType) { \
		res = source; \
		return ExceptionCode::None; \
	} \
	CONVERT(Int8, targetType) \
	CONVERT(Int16, targetType) \
	CONVERT(Int32, targetType) \
	CONVERT(UInt8, targetType) \
	CONVERT(UInt16, targetType) \
	CONVERT(UInt32, targetType) \
	return ExceptionCode::InvalidType; \
}

	TOINT(Int8)
	TOINT(Int16)
	TOINT(Int32)
	TOINT(UInt8)
	TOINT(UInt16)
	TOINT(UInt32)
}
