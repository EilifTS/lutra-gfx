#include "CommandBuffer.h"

namespace efvk
{
	CommandBuffer::CommandBuffer(GraphicsContext::Impl& ctx)
	{
		/* Create command buffer */
		const vk::CommandBufferAllocateInfo allocate_info{
			.commandPool = *ctx.cmd_pool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = 1,
		};
		std::vector<vk::CommandBuffer> cmd_buffers;
		cmd_buf = std::move(ctx.device->allocateCommandBuffersUnique(allocate_info)[0]);
	}
}