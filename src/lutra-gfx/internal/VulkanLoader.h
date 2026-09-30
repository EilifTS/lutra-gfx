#pragma once
#include <string>
#include <volk.h>

namespace lgx
{
	/* Locates and loads the Vulkan loader library, returning its vkGetInstanceProcAddr.
	   Returns nullptr when no loader could be found.

	   The library is opened once and kept loaded for the lifetime of the process, so
	   calling this repeatedly is cheap and always yields the same pointer. Everything
	   that needs to bootstrap Vulkan (GLFW via glfwInitVulkanLoader, volk via
	   volkInitializeCustom, vulkan.hpp's default dispatcher) must be fed this pointer
	   rather than doing its own leaf-name dlopen - see VulkanLoader.cpp for why. */
	PFN_vkGetInstanceProcAddr load_vulkan_loader();

	/* Human readable description of where the loader was searched for, for error messages. */
	std::string vulkan_loader_search_report();
}
