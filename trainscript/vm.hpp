#pragma once
#include <stdlib.h>
#include "module.hpp"

namespace trainscript
{
	class VM
	{
	public:
		Module *load(const void *buffer, size_t length);

		Module *load(const char *text);

		/**
		 * @brief Creates a module by the module name.
		 * @param name The name of the module
		 * @return New module or nullptr if the named module does not exist.
		 */
		virtual Module *create(const ker::String &name);
	};
}
