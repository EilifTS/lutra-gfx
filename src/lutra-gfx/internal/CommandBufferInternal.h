#pragma once
#include <lutra-gfx/GraphicsContext.h>
#include "BufferMemoryAllocator.h"
#include "DescriptorAllocator.h"
#include "DescriptorWriteCache.h"
#include "GraphicsContextInternal.h"
#include "GraphicsPipelineInternal.h"
#include "BufferInternal.h"
#include "TextureInternal.h"
#include "RenderTargetInternal.h"

namespace lgx
{
	class GraphicsPipeline;

	class CommandBufferInternal
	{
	public:
		CommandBufferInternal() {};
		CommandBufferInternal(GraphicsContextInternal& ctx);

		CommandBufferInternal(const CommandBufferInternal&) = delete;
		CommandBufferInternal(CommandBufferInternal&&) = default;
		CommandBufferInternal& operator=(const CommandBufferInternal&) = delete;
		CommandBufferInternal& operator=(CommandBufferInternal&&) = default;

		vk::UniqueCommandBuffer cmd_buf{};

		void BeginRendering(vk::ImageView color_view, vk::ImageView ds_view, u32 width, u32 height, bool clear);
		void EndRendering();

		void BindPipeline(GraphicsPipelineInternal& pipeline);
		void BindBuffer(BufferInternal& buffer, u32 binding);
		void BindTexture(vk::ImageView view, u32 binding);
		void BindTextures(std::span<vk::ImageView> views, u32 binding);

		void ScheduleUpload(const void* src_ptr, u64 size, BufferInternal& dst_buffer);
		void ScheduleUpload(const void* src_ptr, TextureInternal& dst_texture);

		void Barrier(vk::Image image, vk::ImageAspectFlags aspect, ResourceUsage prev_usage, ResourceUsage next_usage);

		void Draw(u32 vertex_count, u32 instance_count);

		void Reset();

	private:
		GraphicsContextInternal* ctx{};

		BufferMemoryAllocator buffer_memory_allocator{};
		DescriptorAllocator descriptor_allocator{};
		DescriptorWriteCache descriptor_write_cache{};

		GraphicsPipelineInternal* bound_pipeline{};

		/* Forces Draw() to allocate and bind a fresh descriptor set even when no new
		   resources were bound, since the previous descriptor set may belong to a
		   different pipeline layout (after BindPipeline) or have been invalidated
		   (after Reset). */
		bool needs_descriptor_set_bind{ true };
	};

	void SubmitAndWaitInternal(GraphicsContextInternal& ctx, CommandBufferInternal& cmd_buf);

	/* Blocking GPU -> CPU readback: submits its own one-off command buffer, waits for it to
	   complete, then copies the result into dst_ptr. Meant for tests/tools, not a hot path -
	   for per-frame work, schedule your own copy via a persistent CommandBuffer instead. */
	void DownloadInternal(GraphicsContextInternal& ctx, BufferInternal& src_buffer, void* dst_ptr, u64 size);
	void DownloadInternal(GraphicsContextInternal& ctx, TextureInternal& src_texture, void* dst_ptr);
	void DownloadInternal(GraphicsContextInternal& ctx, RenderTargetInternal& src_render_target, void* dst_ptr);
}