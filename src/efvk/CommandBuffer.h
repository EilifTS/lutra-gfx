#pragma once
#include "VulkanHPP.h"
#include <efvk/GraphicsContext.h>
#include "GraphicsContextImpl.h"

namespace efvk
{
	class CommandBuffer
	{
	public:
		CommandBuffer() {};
		CommandBuffer(GraphicsContext::Impl& ctx);

		vk::UniqueCommandBuffer cmd_buf{};
	};
}