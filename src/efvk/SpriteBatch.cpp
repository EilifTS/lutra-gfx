#include <efvk/SpriteBatch.h>

#include "internal/FrameManagerInternal.h"
#include "internal/VulkanHPP.h"
#include <efvk/GraphicsPipeline.h>
#include "internal/GraphicsContextInternal.h"

namespace efvk
{
	static constexpr u32 max_sprites_per_batch = 10000;

	struct SpriteBatch::Impl
	{
		vk::Device dev{};
		GraphicsPipeline pipeline{};
	};

	SpriteBatch::SpriteBatch(GraphicsContext& ctx)
	{
		pimpl = std::make_unique<Impl>();

		pimpl->dev = *ctx.internal->device;

		sprite_buffer = Buffer(ctx, max_sprites_per_batch * sizeof(SpriteInternal), BufferType::StorageBuffer);

		/* Create graphics pipeline */
		GraphicsPipelineInfo pipeline_info{};
		pipeline_info.vs_name = "shaders/SpriteBatchShader.vert.spv";
		pipeline_info.ps_name = "shaders/SpriteBatchShader.frag.spv";
		pipeline_info.AddStorageBuffer(0, GraphicsPipelineInfo::Binding::Stage::Vertex);
		pipeline_info.AddImmutableSampler(1, SamplerType::LinearClamp, GraphicsPipelineInfo::Binding::Stage::Fragment);
		pipeline_info.AddTextures(2, max_texture_count, GraphicsPipelineInfo::Binding::Stage::Fragment);

		pimpl->pipeline = GraphicsPipeline(ctx, pipeline_info);

		Image placeholder_img(1, 1);
		placeholder_img.SetData(0, 255);
		placeholder_img.SetData(1, 0);
		placeholder_img.SetData(2, 0);
		placeholder_img.SetData(3, 255);
		placeholder_texture = Texture(ctx, placeholder_img);
	}

	SpriteBatch::~SpriteBatch()
	{

	}

	SpriteBatch::SpriteBatch(SpriteBatch&&) = default;
	SpriteBatch& SpriteBatch::operator=(SpriteBatch&&) = default;

	void SpriteBatch::End(FrameManager& frame_manager)
	{
		CommandBufferInternal& cmd_buf = frame_manager.internal->GetCurrentCommandBuffer();

		if (sprite_list.size() > 0)
		{
			/* Upload sprite data */
			cmd_buf.ScheduleUpload(sprite_list.data(), sprite_list.size() * sizeof(SpriteInternal), *sprite_buffer.internal);

			cmd_buf.BeginRendering(frame_manager.internal->GetCurrentImageView(), frame_manager.internal->window_width, frame_manager.internal->window_height);

			cmd_buf.BindPipeline(*pimpl->pipeline.internal);
			cmd_buf.BindBuffer(*sprite_buffer.internal, 0);
			cmd_buf.BindTextures(*reinterpret_cast<std::array<vk::ImageView, max_texture_count>*>(&texture_view_array), 2);

			cmd_buf.Draw(6, static_cast<u32>(sprite_list.size()));
			
			cmd_buf.EndRendering();
		}
	}
}