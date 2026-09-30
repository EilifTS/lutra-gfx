#include "Diagnostics.h"

#include <cstdlib>
#include <iostream>

namespace lgx
{
	void fatal_error(std::string_view message, const char* file, int line)
	{
		std::cerr << "lutra-gfx fatal error (" << file << ":" << line << "): " << message << std::endl;
		std::abort();
	}
}
