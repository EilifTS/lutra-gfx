#pragma once
#include <string_view>

namespace lgx
{
	/* Reports an unrecoverable initialization/usage error and terminates.
	   Unlike assert() this is active in release builds as well, so a failure
	   produces a readable diagnostic instead of a bare crash. */
	[[noreturn]] void fatal_error(std::string_view message, const char* file, int line);
}

#define LGX_FATAL(message) ::lgx::fatal_error((message), __FILE__, __LINE__)
#define LGX_CHECK(condition, message) do { if (!(condition)) { LGX_FATAL(message); } } while (false)
