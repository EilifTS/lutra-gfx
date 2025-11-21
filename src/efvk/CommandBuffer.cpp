#include "CommandBuffer.h"

namespace efvk
{
	static constexpr u64 chunk_size = 10 * 1024 * 1024; /* 10MB */

	static void buffer_barrier(vk::CommandBuffer cmd_buf, vk::Buffer buffer, u64 offset, u64 size, vk::PipelineStageFlags src_stage, vk::PipelineStageFlags dst_stage, vk::AccessFlags src_access, vk::AccessFlagBits dst_access)
	{
		const vk::BufferMemoryBarrier buffer_barrier{
			.srcAccessMask = src_access,
			.dstAccessMask = dst_access,
			.srcQueueFamilyIndex = vk::QueueFamilyIgnored,
			.dstQueueFamilyIndex = vk::QueueFamilyIgnored,
			.buffer = buffer,
			.offset = offset,
			.size = size,
		};

		cmd_buf.pipelineBarrier(
			src_stage,
			dst_stage,
			vk::DependencyFlagBits::eByRegion,
			{},
			buffer_barrier,
			{}
		);
	}

	CommandBuffer::CommandBuffer(GraphicsContext::Impl& ctx)
		: ctx(&ctx), buffer_memory_allocator(ctx, chunk_size), descriptor_allocator(ctx)
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

	void CommandBuffer::ScheduleUpload(const void* src_ptr, u64 size, Buffer& dst_buffer)
	{
		/* Allocate */
		BufferMemoryAllocation allocation = buffer_memory_allocator.Alloc(*ctx, size, 1);

		/* CPU -> GPU Copy */
		std::memcpy(allocation.ptr, src_ptr, size);

		/* Flush */
		VkResult result = vmaFlushAllocation(ctx->vma_allocator, allocation.vma_allocation, allocation.offset, allocation.size);
		assert(result == VK_SUCCESS);

		/* Barrier */
		buffer_barrier(*cmd_buf, allocation.buffer, allocation.offset, allocation.size, vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eTransfer, vk::AccessFlagBits::eHostWrite, vk::AccessFlagBits::eTransferRead);

		/* GPU -> GPU COPY */
		const vk::BufferCopy buffer_copy{
			.srcOffset = allocation.offset,
			.dstOffset = 0,
			.size = size,
		};

		cmd_buf->copyBuffer(allocation.buffer, dst_buffer.buffer, buffer_copy);
	}

	void CommandBuffer::Reset()
	{
		buffer_memory_allocator.Reset(*ctx);
		descriptor_allocator.Reset();
		cmd_buf->reset(vk::CommandBufferResetFlagBits::eReleaseResources);
	}
}