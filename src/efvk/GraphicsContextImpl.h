#pragma once
#include "VulkanHPP.h"

namespace efvk
{
	struct GraphicsContext::Impl
	{
		vk::UniqueInstance instance{};
#if _DEBUG /* VL */
		vk::UniqueDebugUtilsMessengerEXT messenger{};
#endif
		vk::PhysicalDevice physical_device{};
		vk::UniqueDevice device{};
		vk::Queue queue{};
		u32 queue_family_index{};

		vk::UniqueSurfaceKHR surface{};

		VmaAllocator vma_allocator{};

		vk::UniqueCommandPool cmd_pool{};
	};
}