#include <io.h>
#include "io.hpp"

#include <vmtype.hpp>

namespace csl
{

ExceptionCode outb(VMValue &, const VMArray &args)
{
	if(args.length() != 2)
		return ExceptionCode::InvalidArgument;
	if(args[0].type() != VMType::UInt16)
		return ExceptionCode::InvalidArgument;
	if(args[1].type() != VMType::UInt8)
		return ExceptionCode::InvalidArgument;

	::outb(args[0].value<VMUInt16>(), args[1].value<VMUInt8>());

	return ExceptionCode::None;
}

ExceptionCode inb(VMValue &result, const VMArray &args)
{
	if(args.length() != 1)
		return ExceptionCode::InvalidArgument;
	if(args[0].type() != VMType::UInt16)
		return ExceptionCode::InvalidArgument;

	result = VMValue::UInt8(
		::inb(args[0].value<VMUInt16>()));

	return ExceptionCode::None;
}

}
