#include <efvk/SpriteBatch.h>

#include "FrameManagerImpl.h"
#include "GraphicsContextImpl.h"
#include "VulkanHPP.h"
#include "GraphicsPipeline.h"
#include "Buffer.h"

namespace efvk
{
	static constexpr u32 max_sprites_per_batch = 10000;

	struct SpriteBatch::Impl
	{
		vk::Device dev{};
		Buffer sprite_buffer{};
		GraphicsPipeline pipeline{};
	};

	SpriteBatch::SpriteBatch(GraphicsContext& ctx)
	{
		pimpl = std::make_unique<Impl>();

		pimpl->dev = *ctx.pimpl->device;

		/* Create buffer for sprite data */
		const vk::BufferCreateInfo sprite_buffer_info{
			.size = 0, // TODO: Find out
			.usage = vk::BufferUsageFlagBits::eUniformBuffer,
		};

		pimpl->sprite_buffer = Buffer(*ctx.pimpl, max_sprites_per_batch * sizeof(Sprite), vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst, 0);

		/* Create graphics pipeline */
		GraphicsPipelineInfo pipeline_info{};
		pipeline_info.vs_name = "shaders/SpriteBatchShader.vert.spv";
		pipeline_info.ps_name = "shaders/SpriteBatchShader.frag.spv";
		pipeline_info.AddTexture(0, GraphicsPipelineInfo::Binding::Stage::Fragment);
		pipeline_info.AddImmutableSampler(1, SamplerType::LinearClamp, GraphicsPipelineInfo::Binding::Stage::Fragment);
		pipeline_info.AddStorageBuffer(2, GraphicsPipelineInfo::Binding::Stage::Vertex);

		pimpl->pipeline = GraphicsPipeline(*ctx.pimpl->device, pipeline_info);
	}

	SpriteBatch::~SpriteBatch()
	{

	}

	SpriteBatch::SpriteBatch(SpriteBatch&&) = default;
	SpriteBatch& SpriteBatch::operator=(SpriteBatch&&) = default;

	void SpriteBatch::End(FrameManager& frame_manager, Texture& tex)
	{
		CommandBuffer& cmd_buf = frame_manager.pimpl->GetCurrentCommandBuffer();

		if (sprite_list.size() > 0)
		{
			/* Upload sprite data */
			cmd_buf.ScheduleUpload(sprite_list.data(), sprite_list.size() * sizeof(Sprite), pimpl->sprite_buffer);

			cmd_buf.BeginRendering(frame_manager.pimpl->GetCurrentImageView(), frame_manager.pimpl->window_width, frame_manager.pimpl->window_height);

			cmd_buf.BindPipeline(pimpl->pipeline);
			cmd_buf.BindTexture(tex, 0);
			cmd_buf.BindBuffer(pimpl->sprite_buffer, 2);

			cmd_buf.Draw(6, static_cast<u32>(sprite_list.size()));
			
			cmd_buf.EndRendering();
		}
	}
}