#include "DepthStencilBufferInternal.h"
#include "CommandBufferInternal.h"

#ifdef USE_IMGUI
#include <backends/imgui_impl_vulkan.h>
#endif

namespace efvk
{
	DepthStencilBufferInternal::DepthStencilBufferInternal(GraphicsContextInternal& ctx, u32 width, u32 height, vk::Format format)
	{
		VmaAllocationCreateInfo vma_info{};
		vma_info.usage = VMA_MEMORY_USAGE_AUTO;

		const vk::ImageCreateInfo image_info{
			.imageType = vk::ImageType::e2D,
			.format = format,
			.extent = { width, height, 1 },
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = vk::SampleCountFlagBits::e1,
			.tiling = vk::ImageTiling::eOptimal,
			.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
			.sharingMode = vk::SharingMode::eExclusive,
			.initialLayout = vk::ImageLayout::eUndefined,
		};

		this->width = width;
		this->height = height;
		this->format = format;
		dev = *ctx.device;
		vma_image = VMACreateImage(ctx.vma_allocator, &image_info, &vma_info, nullptr);

		/* Create view */
		const vk::ImageViewCreateInfo view_info{
			.image = vma_image.GetImage(),
			.viewType = vk::ImageViewType::e2D,
			.format = image_info.format,
			.components = {},
			.subresourceRange = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			}
		};

		view = ctx.device->createImageViewUnique(view_info);

		/* Change layout */
		CommandBufferInternal cmd_buf(ctx);
		change_layout(cmd_buf.cmd_buf.get(), vma_image.GetImage(), vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
		SubmitAndWaitInternal(ctx, cmd_buf);
	}
}