#include "CommandBufferInternal.h"
#include "GraphicsPipelineInternal.h"
#include "BufferInternal.h"
#include "TextureInternal.h"

namespace lgx
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

	struct ResourceUsageInfo
	{
		vk::PipelineStageFlags stage;
		vk::AccessFlags access;
	};

	static ResourceUsageInfo GetResourceUsageInfo(ResourceUsage usage)
	{
		switch (usage)
		{
		case ResourceUsage::ColorAttachment:
			return { vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::AccessFlagBits::eColorAttachmentWrite };
		case ResourceUsage::DepthAttachment:
			return { vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests, vk::AccessFlagBits::eDepthStencilAttachmentWrite };
		case ResourceUsage::ShaderRead:
			return { vk::PipelineStageFlagBits::eVertexShader | vk::PipelineStageFlagBits::eFragmentShader, vk::AccessFlagBits::eShaderRead };
		case ResourceUsage::TransferSrc:
			return { vk::PipelineStageFlagBits::eTransfer, vk::AccessFlagBits::eTransferRead };
		case ResourceUsage::TransferDst:
			return { vk::PipelineStageFlagBits::eTransfer, vk::AccessFlagBits::eTransferWrite };
		}
		assert(0);
		return {};
	}

	CommandBufferInternal::CommandBufferInternal(GraphicsContextInternal& ctx)
		: ctx(&ctx), buffer_memory_allocator(ctx, chunk_size), descriptor_allocator(ctx)
	{
		/* Create command buffer */
		const vk::CommandBufferAllocateInfo allocate_info{
			.commandPool = *ctx.cmd_pool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = 1,
		};
		cmd_buf = std::move(VkCheck(ctx.device->allocateCommandBuffersUnique(allocate_info))[0]);

		/* Begin command buffer */
		const vk::CommandBufferBeginInfo begin_info{
			.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
		};

		VkCheck(cmd_buf->begin(begin_info));
	}

	void CommandBufferInternal::BeginRendering(vk::ImageView color_view, vk::ImageView ds_view, u32 width, u32 height, bool clear)
	{
		const std::array<float, 4> clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
		const float clear_depth = 0.0f;

		const vk::ClearValue vk_clear_color{
			.color = clear_color,
		};

		const vk::ClearValue vk_clear_depth{
			.depthStencil = { .depth = clear_depth }
		};

		const vk::RenderingAttachmentInfo color_attachment_info{
			.imageView = color_view,
			.imageLayout = vk::ImageLayout::eGeneral,
			.loadOp = clear ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad,
			.storeOp = vk::AttachmentStoreOp::eStore,
			.clearValue = vk_clear_color,
		};

		const vk::RenderingAttachmentInfo ds_attachment_info{
			.imageView = ds_view,
			.imageLayout = vk::ImageLayout::eGeneral,
			.loadOp = clear ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad,
			.storeOp = vk::AttachmentStoreOp::eStore,
			.clearValue = vk_clear_depth,
		};

		const vk::RenderingInfo rendering_info{
			.renderArea = { 0, 0, width, height},
			.layerCount = 1,
			.colorAttachmentCount = 1,
			.pColorAttachments = &color_attachment_info,
			.pDepthAttachment = ds_view == VK_NULL_HANDLE ? nullptr : &ds_attachment_info,
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

	void CommandBufferInternal::EndRendering()
	{
		cmd_buf->endRendering();
	}

	void CommandBufferInternal::BindPipeline(GraphicsPipelineInternal& pipeline)
	{
		bound_pipeline = &pipeline;
		descriptor_write_cache.Clear();
		needs_descriptor_set_bind = true;
		cmd_buf->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.GetPipeline());
	}

	void CommandBufferInternal::BindBuffer(BufferInternal& buffer, u32 binding)
	{
		descriptor_write_cache.AddBufferWrite(binding, buffer.buffer.GetBuffer(), buffer.Size(), buffer.IsUniform());
	}

	void CommandBufferInternal::BindTexture(vk::ImageView view, u32 binding)
	{
		descriptor_write_cache.AddImageWrite(binding, view);
	}

	void CommandBufferInternal::BindTextures(std::span<vk::ImageView> views, u32 binding)
	{
		descriptor_write_cache.AddImageArrayWrite(binding, views);
	}

	void CommandBufferInternal::ScheduleUpload(const void* src_ptr, u64 size, BufferInternal& dst_buffer)
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

		cmd_buf->copyBuffer(allocation.buffer, dst_buffer.buffer.GetBuffer(), buffer_copy);
	}

	void CommandBufferInternal::ScheduleUpload(const void* src_ptr, TextureInternal& dst_image)
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

	void CommandBufferInternal::Barrier(vk::Image image, vk::ImageAspectFlags aspect, ResourceUsage prev_usage, ResourceUsage next_usage)
	{
		const ResourceUsageInfo prev_info = GetResourceUsageInfo(prev_usage);
		const ResourceUsageInfo next_info = GetResourceUsageInfo(next_usage);

		const vk::ImageSubresourceRange range{
			.aspectMask = aspect,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		};

		/* Every image in this library stays in eGeneral permanently, so this is purely an
		   access-mask/stage barrier - there's no layout to transition. */
		const vk::ImageMemoryBarrier barrier{
			.srcAccessMask = prev_info.access,
			.dstAccessMask = next_info.access,
			.oldLayout = vk::ImageLayout::eGeneral,
			.newLayout = vk::ImageLayout::eGeneral,
			.srcQueueFamilyIndex = vk::QueueFamilyIgnored,
			.dstQueueFamilyIndex = vk::QueueFamilyIgnored,
			.image = image,
			.subresourceRange = range,
		};

		cmd_buf->pipelineBarrier(prev_info.stage, next_info.stage, vk::DependencyFlagBits::eByRegion, {}, {}, barrier);
	}

	void CommandBufferInternal::Draw(u32 vertex_count, u32 instance_count)
	{
		assert(bound_pipeline != nullptr);

		if (needs_descriptor_set_bind || descriptor_write_cache.IsDirty())
		{
			vk::DescriptorSet descriptor_set = descriptor_allocator.Alloc(bound_pipeline->GetDescriptorSetLayout());
			if (descriptor_write_cache.IsDirty())
			{
				descriptor_write_cache.Flush(*ctx->device, descriptor_set);
			}
			cmd_buf->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, bound_pipeline->GetPipelineLayout(), 0, descriptor_set, {});
			needs_descriptor_set_bind = false;
		}

		cmd_buf->draw(vertex_count, instance_count, 0, 0);
	}

	void CommandBufferInternal::Reset()
	{
		buffer_memory_allocator.Reset(*ctx);
		descriptor_allocator.Reset();
		needs_descriptor_set_bind = true;
		VkCheck(cmd_buf->reset(vk::CommandBufferResetFlagBits::eReleaseResources));

		/* Begin command buffer */
		const vk::CommandBufferBeginInfo begin_info{
			.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
		};

		VkCheck(cmd_buf->begin(begin_info));
	}

	void SubmitAndWaitInternal(GraphicsContextInternal& ctx, CommandBufferInternal& cmd_buf)
	{
		vk::UniqueFence fence = VkCheck(ctx.device->createFenceUnique({}));

		/* Submit command buffer */
		VkCheck(cmd_buf.cmd_buf->end());

		const vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eNone;

		const vk::SubmitInfo submit_info{
			.pWaitDstStageMask = &wait_stage,
			.commandBufferCount = 1,
			.pCommandBuffers = &cmd_buf.cmd_buf.get(),
		};
		VkCheck(ctx.queue.submit(submit_info, *fence));

		const vk::Result result = ctx.device->waitForFences(fence.get(), true, UINT64_MAX);
		assert(result == vk::Result::eSuccess);
	}

	void DownloadInternal(GraphicsContextInternal& ctx, BufferInternal& src_buffer, void* dst_ptr, u64 size)
	{
		/* Temporary, host-readable staging buffer to copy the GPU data into */
		BufferInternal staging_buffer(ctx, size, vk::BufferUsageFlagBits::eTransferDst, VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);

		CommandBufferInternal cmd_buf(ctx);

		/* Make sure any prior writes to src_buffer are visible to the copy below */
		buffer_barrier(*cmd_buf.cmd_buf, src_buffer.buffer.GetBuffer(), 0, size, vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eTransfer, vk::AccessFlagBits::eMemoryWrite, vk::AccessFlagBits::eTransferRead);

		const vk::BufferCopy buffer_copy{
			.srcOffset = 0,
			.dstOffset = 0,
			.size = size,
		};
		cmd_buf.cmd_buf->copyBuffer(src_buffer.buffer.GetBuffer(), staging_buffer.buffer.GetBuffer(), buffer_copy);

		SubmitAndWaitInternal(ctx, cmd_buf);

		/* CPU read: invalidate first in case the staging memory ended up non-coherent */
		void* mapped_ptr = staging_buffer.Map(*ctx.device);
		VkResult result = vmaInvalidateAllocation(ctx.vma_allocator, staging_buffer.buffer.GetAllocation(), 0, size);
		assert(result == VK_SUCCESS);
		std::memcpy(dst_ptr, mapped_ptr, size);
		staging_buffer.Unmap(*ctx.device);
	}

	/* Shared by the Texture/RenderTarget download overloads below - both are plain
	   eR8G8B8A8Unorm, eGeneral-layout color images, so the copy is identical either way. */
	static void DownloadColorImageInternal(GraphicsContextInternal& ctx, vk::Image image, u32 width, u32 height, void* dst_ptr)
	{
		const u64 bytes_per_pixel = 4; /* Matches the format these images always use today */
		const u64 size = static_cast<u64>(width) * height * bytes_per_pixel;

		/* Temporary, host-readable staging buffer to copy the GPU data into */
		BufferInternal staging_buffer(ctx, size, vk::BufferUsageFlagBits::eTransferDst, VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);

		CommandBufferInternal cmd_buf(ctx);

		/* Make sure any prior writes to the image are visible to the copy below. These images
		   always sit in eGeneral, so this is purely an access-mask barrier. */
		const vk::ImageSubresourceRange range{
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		};
		const vk::ImageMemoryBarrier pre_copy_barrier{
			.srcAccessMask = vk::AccessFlagBits::eMemoryWrite,
			.dstAccessMask = vk::AccessFlagBits::eTransferRead,
			.oldLayout = vk::ImageLayout::eGeneral,
			.newLayout = vk::ImageLayout::eGeneral,
			.srcQueueFamilyIndex = vk::QueueFamilyIgnored,
			.dstQueueFamilyIndex = vk::QueueFamilyIgnored,
			.image = image,
			.subresourceRange = range,
		};
		cmd_buf.cmd_buf->pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlagBits::eByRegion, {}, {}, pre_copy_barrier);

		const vk::BufferImageCopy buffer_image_copy{
			.bufferOffset = 0,
			.imageSubresource = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
			.imageExtent = { width, height, 1 },
		};
		cmd_buf.cmd_buf->copyImageToBuffer(image, vk::ImageLayout::eGeneral, staging_buffer.buffer.GetBuffer(), buffer_image_copy);

		SubmitAndWaitInternal(ctx, cmd_buf);

		void* mapped_ptr = staging_buffer.Map(*ctx.device);
		VkResult result = vmaInvalidateAllocation(ctx.vma_allocator, staging_buffer.buffer.GetAllocation(), 0, size);
		assert(result == VK_SUCCESS);
		std::memcpy(dst_ptr, mapped_ptr, size);
		staging_buffer.Unmap(*ctx.device);
	}

	void DownloadInternal(GraphicsContextInternal& ctx, TextureInternal& src_texture, void* dst_ptr)
	{
		DownloadColorImageInternal(ctx, src_texture.vma_image.GetImage(), src_texture.width, src_texture.height, dst_ptr);
	}

	void DownloadInternal(GraphicsContextInternal& ctx, RenderTargetInternal& src_render_target, void* dst_ptr)
	{
		DownloadColorImageInternal(ctx, src_render_target.vma_image.GetImage(), src_render_target.width, src_render_target.height, dst_ptr);
	}
}