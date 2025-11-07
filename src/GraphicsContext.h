#pragma once

#define VULKAN_HPP_TYPESAFE_CONVERSION 1
#include <vulkan/vulkan.hpp>

#include "Window.h"

namespace efvk
{
	class GraphicsContext
	{
	public:
		GraphicsContext(const Window& window);

	private:
		vk::Instance instance;
		vk::PhysicalDevice physical_device;
		vk::Device device;

		vk::SurfaceKHR surface;
	};
}