#include "Texture3DInternal.h"
#include "CommandBufferInternal.h"
#include "CommonHelpers.h"

namespace lgx
{
	static vk::UniqueImageView create_view(vk::Device dev, vk::Image image, vk::Format format, u32 base_mip, u32 level_count)
	{
		const vk::ImageViewCreateInfo view_info{
			.image = image,
			.viewType = vk::ImageViewType::e3D,
			.format = format,
			.components = {},
			.subresourceRange = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.baseMipLevel = base_mip,
				.levelCount = level_count,
				.baseArrayLayer = 0,
				.layerCount = 1,
			}
		};
		return VkCheck(dev.createImageViewUnique(view_info));
	}

	Texture3DInternal::Texture3DInternal(GraphicsContextInternal& ctx, const Texture3DInfo& info)
		: texture_format(info.format), width(info.width), height(info.height), depth(info.depth), mip_count(info.mip_count)
	{
		assert(width > 0 && height > 0 && depth > 0);
		assert(mip_count >= 1 && mip_count <= CalculateMipCount(width, height, depth));

		const vk::Format format = convert_texture_format(info.format);

		vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst;
		if (info.storage)
		{
			usage |= vk::ImageUsageFlagBits::eStorage;
		}

		const vk::FormatProperties format_properties = ctx.physical_device.getFormatProperties(format);
		assert(format_properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImage);
		assert(!info.storage || (format_properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eStorageImage));

		VmaAllocationCreateInfo vma_info{};
		vma_info.usage = VMA_MEMORY_USAGE_AUTO;

		const vk::ImageCreateInfo image_info{
			.imageType = vk::ImageType::e3D,
			.format = format,
			.extent = { width, height, depth },
			.mipLevels = mip_count,
			.arrayLayers = 1,
			.samples = vk::SampleCountFlagBits::e1,
			.tiling = vk::ImageTiling::eOptimal,
			.usage = usage,
			.sharingMode = vk::SharingMode::eExclusive,
			.initialLayout = vk::ImageLayout::eUndefined,
		};
		vma_image = VMACreateImage(ctx.vma_allocator, &image_info, &vma_info, nullptr);

		view = create_view(*ctx.device, vma_image.GetImage(), format, 0, mip_count);
		for (u32 mip = 0; mip < mip_count; mip++)
		{
			mip_views.push_back(create_view(*ctx.device, vma_image.GetImage(), format, mip, 1));
		}

		/* Like every image in this library it stays in General, so it can be used for anything without transitions */
		CommandBufferInternal cmd_buf(ctx);
		change_layout(cmd_buf.cmd_buf.get(), vma_image.GetImage(), vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral, vk::ImageAspectFlagBits::eColor, mip_count);
		SubmitAndWaitInternal(ctx, cmd_buf);
	}
}
