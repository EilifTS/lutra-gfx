#include "VulkanLoader.h"

#include <cstdlib>
#include <vector>

#if defined(_WIN32)
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#else
#	include <dlfcn.h>
#endif

/* Why this file exists:

   GLFW (src/vulkan.c), volk and vulkan.hpp's DynamicLoader all locate the Vulkan loader by
   doing a leaf-name dlopen ("libvulkan.1.dylib" / "libvulkan.so.1" / "vulkan-1.dll").
   On current macOS the dyld fallback search for a leaf name is effectively just /usr/lib -
   neither /usr/local/lib (LunarG) nor /opt/homebrew/lib (brew vulkan-loader) is consulted:

       dlopen(libvulkan.1.dylib, 0x0005): tried: 'libvulkan.1.dylib' (no such file),
       '/System/Volumes/Preboot/Cryptexes/OSlibvulkan.1.dylib', '/usr/lib/libvulkan.1.dylib'

   so glfwVulkanSupported() returns false unless the user exports DYLD_FALLBACK_LIBRARY_PATH.
   We instead try a list of absolute candidate paths ourselves and hand the resulting
   vkGetInstanceProcAddr to GLFW/volk/vulkan.hpp, which removes the env-var requirement and
   makes app bundles self-contained (the @executable_path/@loader_path candidates). */

namespace
{
	std::vector<std::string> loader_candidates()
	{
		std::vector<std::string> candidates;

		/* An explicit override always wins */
		if (const char* override_path = std::getenv("LGX_VULKAN_LOADER"))
		{
			candidates.emplace_back(override_path);
		}

#if defined(_WIN32)
		/* The Windows loader lives in the system directory and is found by name */
		candidates.emplace_back("vulkan-1.dll");
		if (const char* sdk = std::getenv("VULKAN_SDK"))
		{
			candidates.emplace_back(std::string(sdk) + "\\Bin\\vulkan-1.dll");
		}
#elif defined(__APPLE__)
		/* Next to / bundled with the executable, so a packaged .app or archive is self-contained */
		candidates.emplace_back("@executable_path/../Frameworks/libvulkan.1.dylib");
		candidates.emplace_back("@executable_path/libvulkan.1.dylib");
		candidates.emplace_back("@loader_path/libvulkan.1.dylib");

		if (const char* sdk = std::getenv("VULKAN_SDK"))
		{
			candidates.emplace_back(std::string(sdk) + "/lib/libvulkan.1.dylib");
		}

		/* Default dyld search - honours DYLD_LIBRARY_PATH/DYLD_FALLBACK_LIBRARY_PATH and rpaths */
		candidates.emplace_back("libvulkan.1.dylib");
		candidates.emplace_back("libvulkan.dylib");

		/* The install locations dyld refuses to search on its own */
		candidates.emplace_back("/usr/local/lib/libvulkan.1.dylib");
		candidates.emplace_back("/usr/local/lib/libvulkan.dylib");
		candidates.emplace_back("/opt/homebrew/lib/libvulkan.1.dylib");
		candidates.emplace_back("/opt/homebrew/lib/libvulkan.dylib");

		if (const char* home = std::getenv("HOME"))
		{
			candidates.emplace_back(std::string(home) + "/.local/lib/libvulkan.1.dylib");
		}
#else
		candidates.emplace_back("libvulkan.so.1");
		candidates.emplace_back("libvulkan.so");
		if (const char* sdk = std::getenv("VULKAN_SDK"))
		{
			candidates.emplace_back(std::string(sdk) + "/lib/libvulkan.so.1");
		}
#endif

		return candidates;
	}

	void* open_library(const char* path)
	{
#if defined(_WIN32)
		return static_cast<void*>(LoadLibraryA(path));
#else
		return dlopen(path, RTLD_NOW | RTLD_LOCAL);
#endif
	}

	void* lookup_symbol(void* library, const char* name)
	{
#if defined(_WIN32)
		return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(library), name));
#else
		return dlsym(library, name);
#endif
	}

	struct LoaderResult
	{
		PFN_vkGetInstanceProcAddr get_instance_proc_addr{};
		std::string path{};
	};

	const LoaderResult& resolve_loader()
	{
		/* Loaded once; the library is deliberately never unloaded */
		static const LoaderResult result = []
		{
			for (const std::string& candidate : loader_candidates())
			{
				void* library = open_library(candidate.c_str());
				if (library == nullptr)
				{
					continue;
				}

				auto get_instance_proc_addr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(
					lookup_symbol(library, "vkGetInstanceProcAddr"));
				if (get_instance_proc_addr == nullptr)
				{
					continue;
				}

				return LoaderResult{ get_instance_proc_addr, candidate };
			}

			return LoaderResult{};
		}();

		return result;
	}
}

namespace lgx
{
	PFN_vkGetInstanceProcAddr load_vulkan_loader()
	{
		return resolve_loader().get_instance_proc_addr;
	}

	std::string vulkan_loader_search_report()
	{
		const LoaderResult& result = resolve_loader();
		if (result.get_instance_proc_addr != nullptr)
		{
			return "Vulkan loader: " + result.path;
		}

		std::string report = "No Vulkan loader found. Tried:";
		for (const std::string& candidate : loader_candidates())
		{
			report += "\n  " + candidate;
		}
		report += "\nInstall the Vulkan SDK, or point LGX_VULKAN_LOADER at the loader library.";
		return report;
	}
}
