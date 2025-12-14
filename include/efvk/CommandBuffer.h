#pragma once
#include <memory>
#include <span>
#include <efvk/math/IntDefs.h>
#include "Texture.h"

namespace efvk
{
	class CommandBufferInternal;

	class Buffer;
	class GraphicsContext;
	class GraphicsPipeline;

	class CommandBuffer
	{
	public:
		CommandBuffer();
		CommandBuffer(GraphicsContext& ctx);
		~CommandBuffer();

		CommandBuffer(const CommandBuffer&) = delete;
		CommandBuffer(CommandBuffer&&);
		CommandBuffer& operator=(const CommandBuffer&) = delete;
		CommandBuffer& operator=(CommandBuffer&&);

		void BeginRendering(TextureView color_view, TextureView ds_view, u32 width, u32 height, bool clear);
		void EndRendering();

		void BindPipeline(GraphicsPipeline& pipeline);
		void BindBuffer(Buffer& buffer, u32 binding);
		void BindTexture(TextureView texture, u32 binding);
		void BindTextures(std::span<TextureView> textures, u32 binding);

		void ScheduleUpload(const void* src_ptr, u64 size, Buffer& dst_buffer);
		void ScheduleUpload(const void* src_ptr, Texture& dst_texture);

		void Draw(u32 vertex_count, u32 instance_count);

		void Reset();

		std::unique_ptr<CommandBufferInternal> internal{};
	};

	void SubmitAndWait(GraphicsContext& ctx, CommandBuffer& cmd_buf);
}