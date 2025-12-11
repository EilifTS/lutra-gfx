#include "CommandBuffer.h"
#include "GraphicsPipeline.h"
#include "internal/BufferInternal.h"
#include "internal/TextureInternal.h"

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

	CommandBuffer::CommandBuffer(GraphicsContextInternal& ctx)
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

		/* Begin command buffer */
		const vk::CommandBufferBeginInfo begin_info{
			.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
		};

		cmd_buf->begin(begin_info);
	}

	void CommandBuffer::BeginRendering(vk::ImageView image_view, u32 width, u32 height)
	{
		const vk::RenderingAttachmentInfo color_attachment_info{
				.imageView = image_view,
				.imageLayout = vk::ImageLayout::eGeneral,
				.loadOp = vk::AttachmentLoadOp::eLoad,
				.storeOp = vk::AttachmentStoreOp::eStore,
		};

		const vk::RenderingInfo rendering_info{
			.renderArea = { 0, 0, width, height},
			.layerCount = 1,
			.colorAttachmentCount = 1,
			.pColorAttachments = &color_attachment_info,
		};

		cmd_buf->beginRendering(rendering_info);

		const vk::Viewport viewport{
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>(width),
			.height = static_cast<float>(height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f,
		};

		const vk::Rect2D scissor{
			.offset = { 0, 0 },
			.extent = { width, height },
		};

		cmd_buf->setViewport(0, viewport);
		cmd_buf->setScissor(0, scissor);
	}

	void CommandBuffer::EndRendering()
	{
		cmd_buf->endRendering();
	}

	void CommandBuffer::BindPipeline(GraphicsPipeline& pipeline)
	{
		bound_pipeline = &pipeline;
		descriptor_write_cache.Clear();
		cmd_buf->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.GetPipeline());
	}

	void CommandBuffer::BindBuffer(Buffer& buffer, u32 binding)
	{
		descriptor_write_cache.AddBufferWrite(binding, buffer.internal->buffer.GetBuffer(), buffer.Size());
	}

	void CommandBuffer::BindTexture(Texture& texture, u32 binding)
	{
		descriptor_write_cache.AddImageWrite(binding, *texture.internal->view);
	}

	void CommandBuffer::BindTextures(std::span<Texture*> textures, u32 binding)
	{
		/* TODO: Remove vector usage? */
		std::vector<vk::ImageView> views(textures.size());
		for (u32 i = 0; i < static_cast<u32>(textures.size()); i++)
		{
			views[i] = *textures[i]->internal->view;
		}
		descriptor_write_cache.AddImageArrayWrite(binding, views);
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

		cmd_buf->copyBuffer(allocation.buffer, dst_buffer.internal->buffer.GetBuffer(), buffer_copy);
	}

	void CommandBuffer::ScheduleUpload(const void* src_ptr, Texture& dst_image)
	{
		ScheduleUpload(src_ptr, *dst_image.internal);
	}

	void CommandBuffer::ScheduleUpload(const void* src_ptr, TextureInternal& dst_image)
	{
		/* Calculate buffer size */
		const u64 bytes_per_pixel = 4; /* 4bytes per pixel, have to do this properly at some point */
		const u64 buffer_size = dst_image.width * dst_image.height * bytes_per_pixel;

		/* Allocate */
		BufferMemoryAllocation allocation = buffer_memory_allocator.Alloc(*ctx, buffer_size, 1);

		/* CPU -> GPU Copy */
		std::memcpy(allocation.ptr, src_ptr, buffer_size);

		/* Flush */
		VkResult result = vmaFlushAllocation(ctx->vma_allocator, allocation.vma_allocation, allocation.offset, allocation.size);
		assert(result == VK_SUCCESS);

		/* Barrier */
		buffer_barrier(*cmd_buf, allocation.buffer, allocation.offset, allocation.size, vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eTransfer, vk::AccessFlagBits::eHostWrite, vk::AccessFlagBits::eTransferRead);

		/* GPU -> GPU COPY */
		const vk::BufferImageCopy buffer_image_copy{
			.bufferOffset = allocation.offset,
			.imageSubresource = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
			.imageExtent = { dst_image.width, dst_image.height, 1 },
		};

		cmd_buf->copyBufferToImage(allocation.buffer, dst_image.vma_image.GetImage(), vk::ImageLayout::eGeneral, buffer_image_copy);
	}

	void CommandBuffer::Draw(u32 vertex_count, u32 instance_count)
	{
		assert(bound_pipeline != nullptr);

		if (descriptor_write_cache.IsDirty())
		{
			vk::DescriptorSet descriptor_set = descriptor_allocator.Alloc(bound_pipeline->GetDescriptorSetLayout());
			descriptor_write_cache.Flush(*ctx->device, descriptor_set);
			cmd_buf->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, bound_pipeline->GetPipelineLayout(), 0, descriptor_set, {});
		}

		cmd_buf->draw(vertex_count, instance_count, 0, 0);
	}

	void CommandBuffer::Reset()
	{
		buffer_memory_allocator.Reset(*ctx);
		descriptor_allocator.Reset();
		cmd_buf->reset(vk::CommandBufferResetFlagBits::eReleaseResources);

		/* Begin command buffer */
		const vk::CommandBufferBeginInfo begin_info{
			.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
		};

		cmd_buf->begin(begin_info);
	}

	void SubmitAndWait(GraphicsContextInternal& ctx, CommandBuffer& cmd_buf)
	{
		vk::UniqueFence fence = ctx.device->createFenceUnique({});

		/* Submit command buffer */
		cmd_buf.cmd_buf->end();

		const vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eNone;

		const vk::SubmitInfo submit_info{
			.pWaitDstStageMask = &wait_stage,
			.commandBufferCount = 1,
			.pCommandBuffers = &cmd_buf.cmd_buf.get(),
		};
		ctx.queue.submit(submit_info, *fence);

		const vk::Result result = ctx.device->waitForFences(fence.get(), true, UINT64_MAX);
		assert(result == vk::Result::eSuccess);
	}
}