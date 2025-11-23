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
		pimpl->pipeline = GraphicsPipeline(*ctx.pimpl->device, "shaders/SpriteBatchShader.vert.spv", "shaders/SpriteBatchShader.frag.spv");
		pimpl->pipeline.Compile(*ctx.pimpl->device);
	}

	SpriteBatch::~SpriteBatch()
	{

	}

	SpriteBatch::SpriteBatch(SpriteBatch&&) = default;
	SpriteBatch& SpriteBatch::operator=(SpriteBatch&&) = default;

	void SpriteBatch::End(FrameManager& frame_manager)
	{
		CommandBuffer& cmd_buf = frame_manager.pimpl->GetCurrentCommandBuffer();

		if (sprite_list.size() > 0)
		{
			/* Upload sprite data */
			cmd_buf.ScheduleUpload(sprite_list.data(), sprite_list.size() * sizeof(Sprite), pimpl->sprite_buffer);

			vk::DescriptorSet set = cmd_buf.AllocateDescriptorSet(pimpl->pipeline.GetDescriptorSetLayout());

			const vk::DescriptorBufferInfo desc_buffer_info{
				.buffer = pimpl->sprite_buffer.buffer.GetBuffer(),
				.range = pimpl->sprite_buffer.Size(),
			};

			const vk::WriteDescriptorSet desc_writes{
				.dstSet = set,
				.dstBinding = 0,
				.descriptorCount = 1,
				.descriptorType = vk::DescriptorType::eStorageBuffer,
				.pBufferInfo = &desc_buffer_info,
			};
			pimpl->dev.updateDescriptorSets(desc_writes, {});

			const vk::RenderingAttachmentInfo color_attachment_info{
				.imageView = frame_manager.pimpl->GetCurrentImageView(),
				.imageLayout = vk::ImageLayout::eGeneral,
				.loadOp = vk::AttachmentLoadOp::eLoad,
				.storeOp = vk::AttachmentStoreOp::eStore,
			};

			const u32 width = frame_manager.pimpl->window_width;
			const u32 height = frame_manager.pimpl->window_height;
			const vk::RenderingInfo rendering_info{
				.renderArea = { 0, 0, width, height},
				.layerCount = 1,
				.colorAttachmentCount = 1,
				.pColorAttachments = &color_attachment_info,
			};

			cmd_buf.cmd_buf->beginRendering(rendering_info);
			cmd_buf.cmd_buf->bindPipeline(vk::PipelineBindPoint::eGraphics, pimpl->pipeline.GetPipeline());
			cmd_buf.cmd_buf->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pimpl->pipeline.GetPipelineLayout(), 0, set, {});

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

			cmd_buf.cmd_buf->setViewport(0, viewport);
			cmd_buf.cmd_buf->setScissor(0, scissor);

			cmd_buf.cmd_buf->draw(6, static_cast<u32>(sprite_list.size()), 0, 0);
			
			cmd_buf.cmd_buf->endRendering();
		}
	}
}