#pragma once
#include <memory>
#include <span>
#include "CommonStructs.h"
#include "Texture.h"

namespace lgx
{
	class CommandBufferInternal;

	class Buffer;
	class GraphicsContext;
	class GraphicsPipeline;
	class ComputePipeline;
	class DepthStencilBuffer;
	class RenderTarget;

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
		void BindPipeline(ComputePipeline& pipeline);
		void BindBuffer(Buffer& buffer, u32 binding);
		void BindTexture(TextureView texture, u32 binding);
		void BindTextures(std::span<TextureView> textures, u32 binding);

		/* Binds a texture/render target for a compute shader to write via imageStore. The
		   resource must have been created with storage-image support (RenderTarget already
		   is; Texture is not, since it's meant to hold data uploaded from the CPU). */
		void BindStorageImage(TextureView texture, u32 binding);

		void ScheduleUpload(const void* src_ptr, u64 size, Buffer& dst_buffer);
		void ScheduleUpload(const void* src_ptr, Texture& dst_texture);

		/* Synchronizes access to a resource between what it was just used for and what it's
		   about to be used for - e.g. Barrier(rt, ResourceUsage::ColorAttachment,
		   ResourceUsage::ShaderRead) after rendering into a texture and before sampling it. */
		void Barrier(Texture& texture, ResourceUsage prev_usage, ResourceUsage next_usage);
		void Barrier(DepthStencilBuffer& depth_stencil_buffer, ResourceUsage prev_usage, ResourceUsage next_usage);
		void Barrier(RenderTarget& render_target, ResourceUsage prev_usage, ResourceUsage next_usage);
		void Barrier(Buffer& buffer, ResourceUsage prev_usage, ResourceUsage next_usage);

		void Draw(u32 vertex_count, u32 instance_count);
		void Dispatch(u32 x, u32 y, u32 z);

		void Reset();

		std::unique_ptr<CommandBufferInternal> internal{};
	};

	void SubmitAndWait(GraphicsContext& ctx, CommandBuffer& cmd_buf);

	/* Blocking GPU -> CPU readback, handy for tests/tools: submits its own one-off command
	   buffer and waits for it, then copies the result into dst_ptr. Not meant for per-frame
	   use - for that, schedule your own copy through a persistent CommandBuffer instead.
	   dst_ptr must point at a buffer at least `size` bytes for the Buffer overload, or at
	   least Width() * Height() * 4 bytes for the Texture overload. */
	void Download(GraphicsContext& ctx, Buffer& src_buffer, void* dst_ptr, u64 size);
	void Download(GraphicsContext& ctx, Texture& src_texture, void* dst_ptr);
	void Download(GraphicsContext& ctx, RenderTarget& src_render_target, void* dst_ptr);
}