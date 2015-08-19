#pragma once
#include <stdlib.h>
#include "module.hpp"

namespace trainscript
{
	class VM
	{
	public:
		static Module *load(const void *buffer, size_t length);

		static Module *load(const char *text);
	};
}
