#include <lutra-gfx/CommandBuffer.h>
#include <lutra-gfx/Buffer.h>
#include <lutra-gfx/Texture.h>
#include <lutra-gfx/DepthStencilBuffer.h>
#include <lutra-gfx/RenderTarget.h>
#include <lutra-gfx/ComputePipeline.h>
#include "internal/GraphicsContextInternal.h"
#include "internal/CommandBufferInternal.h"
#include "internal/DepthStencilBufferInternal.h"
#include "internal/RenderTargetInternal.h"
#include "internal/ComputePipelineInternal.h"
#include <vector>

namespace lgx
{
	CommandBuffer::CommandBuffer() {}
	CommandBuffer::CommandBuffer(GraphicsContext& ctx)
	{
		internal = std::make_unique<CommandBufferInternal>(*ctx.internal);
	}

	CommandBuffer::~CommandBuffer() {}

	CommandBuffer::CommandBuffer(CommandBuffer&&) = default;
	CommandBuffer& CommandBuffer::operator=(CommandBuffer&&) = default;

	void CommandBuffer::BeginRendering(TextureView color_view, TextureView ds_view, u32 width, u32 height, bool clear)
	{
		internal->BeginRendering(*reinterpret_cast<vk::ImageView*>(&color_view), *reinterpret_cast<vk::ImageView*>(&ds_view), width, height, clear);
	}

	void CommandBuffer::EndRendering() { internal->EndRendering(); }

	void CommandBuffer::BindPipeline(GraphicsPipeline& pipeline) { internal->BindPipeline(*pipeline.internal); };
	void CommandBuffer::BindPipeline(ComputePipeline& pipeline) { internal->BindComputePipeline(*pipeline.internal); };
	void CommandBuffer::BindBuffer(Buffer& buffer, u32 binding) { internal->BindBuffer(*buffer.internal, binding); };
	void CommandBuffer::BindTexture(TextureView view, u32 binding) { internal->BindTexture(*reinterpret_cast<vk::ImageView*>(&view), binding); };
	void CommandBuffer::BindTextures(std::span<TextureView> views, u32 binding)
	{
		std::span<vk::ImageView>* vk_views = reinterpret_cast<std::span<vk::ImageView>*>(&views);
		internal->BindTextures(*vk_views, binding);
	};
	void CommandBuffer::BindStorageImage(TextureView texture, u32 binding) { internal->BindStorageImage(*reinterpret_cast<vk::ImageView*>(&texture), binding); };

	void CommandBuffer::ScheduleUpload(const void* src_ptr, u64 size, Buffer& dst_buffer) { internal->ScheduleUpload(src_ptr, size, *dst_buffer.internal); };
	void CommandBuffer::ScheduleUpload(const void* src_ptr, Texture& dst_texture) { internal->ScheduleUpload(src_ptr, *dst_texture.internal); };

	void CommandBuffer::Barrier(Texture& texture, ResourceUsage prev_usage, ResourceUsage next_usage)
	{
		internal->Barrier(texture.internal->vma_image.GetImage(), vk::ImageAspectFlagBits::eColor, prev_usage, next_usage);
	}
	void CommandBuffer::Barrier(DepthStencilBuffer& depth_stencil_buffer, ResourceUsage prev_usage, ResourceUsage next_usage)
	{
		internal->Barrier(depth_stencil_buffer.internal->vma_image.GetImage(), vk::ImageAspectFlagBits::eDepth, prev_usage, next_usage);
	}
	void CommandBuffer::Barrier(RenderTarget& render_target, ResourceUsage prev_usage, ResourceUsage next_usage)
	{
		internal->Barrier(render_target.internal->vma_image.GetImage(), vk::ImageAspectFlagBits::eColor, prev_usage, next_usage);
	}
	void CommandBuffer::Barrier(Buffer& buffer, ResourceUsage prev_usage, ResourceUsage next_usage)
	{
		internal->Barrier(buffer.internal->buffer.GetBuffer(), prev_usage, next_usage);
	}

	void CommandBuffer::Draw(u32 vertex_count, u32 instance_count) { internal->Draw(vertex_count, instance_count); };
	void CommandBuffer::Dispatch(u32 x, u32 y, u32 z) { internal->Dispatch(x, y, z); };

	void CommandBuffer::Reset() { internal->Reset(); };

	void SubmitAndWait(GraphicsContext& ctx, CommandBuffer& cmd_buf)
	{
		SubmitAndWaitInternal(*ctx.internal, *cmd_buf.internal);
	}

	void Download(GraphicsContext& ctx, Buffer& src_buffer, void* dst_ptr, u64 size)
	{
		DownloadInternal(*ctx.internal, *src_buffer.internal, dst_ptr, size);
	}

	void Download(GraphicsContext& ctx, Texture& src_texture, void* dst_ptr)
	{
		DownloadInternal(*ctx.internal, *src_texture.internal, dst_ptr);
	}

	void Download(GraphicsContext& ctx, RenderTarget& src_render_target, void* dst_ptr)
	{
		DownloadInternal(*ctx.internal, *src_render_target.internal, dst_ptr);
	}
}