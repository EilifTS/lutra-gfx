#pragma once
#include <efvk/GraphicsContext.h>
#include <efvk/Texture.h>
#include <efvk/Buffer.h>
#include "../BufferMemoryAllocator.h"
#include "../DescriptorAllocator.h"
#include "../DescriptorWriteCache.h"
#include "GraphicsContextInternal.h"
#include "GraphicsPipelineInternal.h"

namespace efvk
{
	class GraphicsPipeline;
	/* TODO: remove this WA */
	class TextureInternal;

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

		void BeginRendering(vk::ImageView image_view, u32 width, u32 height);
		void EndRendering();

		void BindPipeline(GraphicsPipelineInternal& pipeline);
		void BindBuffer(BufferInternal& buffer, u32 binding);
		void BindTexture(vk::ImageView view, u32 binding);
		void BindTextures(std::span<vk::ImageView> views, u32 binding);

		void ScheduleUpload(const void* src_ptr, u64 size, Buffer& dst_buffer);
		void ScheduleUpload(const void* src_ptr, Texture& dst_texture);

		/* TODO: remove this WA */
		void ScheduleUpload(const void* src_ptr, TextureInternal& dst_texture);

		void Draw(u32 vertex_count, u32 instance_count);

		void Reset();

	private:
		GraphicsContextInternal* ctx{};

		BufferMemoryAllocator buffer_memory_allocator{};
		DescriptorAllocator descriptor_allocator{};
		DescriptorWriteCache descriptor_write_cache{};

		GraphicsPipelineInternal* bound_pipeline{};
	};

	void SubmitAndWaitInternal(GraphicsContextInternal& ctx, CommandBufferInternal& cmd_buf);
}