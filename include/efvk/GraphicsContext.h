#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_TYPESAFE_CONVERSION 1
#include <vulkan/vulkan.hpp>

#include "Window.h"

namespace efvk
{
	class GraphicsContext
	{
	public:
		GraphicsContext(const Window& window, const char* app_name);
		~GraphicsContext();

		vk::UniqueInstance instance;
#if _DEBUG /* VL */
		vk::UniqueDebugUtilsMessengerEXT messenger;
#endif
		vk::PhysicalDevice physical_device;
		vk::UniqueDevice device;
		vk::Queue queue;
		u32 queue_family_index;

		vk::UniqueSurfaceKHR surface;

	private:
		
	};
}