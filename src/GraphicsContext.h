#pragma once
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
	};
}