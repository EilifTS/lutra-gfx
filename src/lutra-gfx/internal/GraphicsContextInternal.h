#pragma once
#include <lutra-gfx/Window.h>
#include "VulkanHPP.h"

namespace lgx
{
	class GraphicsContextInternal
	{
	public:
		GraphicsContextInternal(const char* app_name);
		GraphicsContextInternal(const char* app_name, const Window& window);
		~GraphicsContextInternal();

		void WaitIdle();

		vk::UniqueInstance instance{};
#ifndef NDEBUG /* VL */
		vk::UniqueDebugUtilsMessengerEXT messenger{};
#endif
		vk::PhysicalDevice physical_device{};
		vk::UniqueDevice device{};
		vk::Queue queue{};
		u32 queue_family_index{};

		vk::UniqueSurfaceKHR surface{};
		/* Format the swapchain uses (always sRGB-nonlinear color space). Only meaningful when there is a window. */
		ColorFormat swapchain_color_format{};

		VmaAllocator vma_allocator{};

		vk::UniqueCommandPool cmd_pool{};

	private:
		void init(const char* app_name, const Window* window);
	};
}
