#include "TextureInternal.h"
#include "CommandBufferInternal.h"

#ifdef USE_IMGUI
#include <backends/imgui_impl_vulkan.h>
#endif

namespace efvk
{
	TextureInternal::TextureInternal(GraphicsContextInternal& ctx, const Image& image)
	{
		VmaAllocationCreateInfo vma_info{};
		vma_info.usage = VMA_MEMORY_USAGE_AUTO;

		const vk::ImageCreateInfo image_info{
			.imageType = vk::ImageType::e2D,
			.format = vk::Format::eR8G8B8A8Unorm,
			.extent = { image.Width(), image.Height(), 1 },
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = vk::SampleCountFlagBits::e1,
			.tiling = vk::ImageTiling::eOptimal,
			.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
			.sharingMode = vk::SharingMode::eExclusive,
			.initialLayout = vk::ImageLayout::eUndefined,
		};

		width = image.Width();
		height = image.Height();
		format = vk::Format::eR8G8B8A8Unorm;
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

		/* Initialize ImGui resources */
#ifdef USE_IMGUI
		const vk::SamplerCreateInfo sampler_info{
			.magFilter = vk::Filter::eNearest,
			.minFilter = vk::Filter::eNearest,
			.addressModeU = vk::SamplerAddressMode::eClampToEdge,
			.addressModeV = vk::SamplerAddressMode::eClampToEdge,
			.addressModeW = vk::SamplerAddressMode::eClampToEdge,
			.maxLod = vk::LodClampNone,
		};
		imgui_sampler = ctx.device->createSamplerUnique(sampler_info);

		imgui_set = ImGui_ImplVulkan_AddTexture(*imgui_sampler, *view, VK_IMAGE_LAYOUT_GENERAL);
#endif

		/* Initialize content */
		CommandBufferInternal cmd_buf(ctx);
		change_layout(cmd_buf.cmd_buf.get(), vma_image.GetImage(), vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral, vk::ImageAspectFlagBits::eColor);
		cmd_buf.ScheduleUpload(image.GetDataPtr(), *this);
		SubmitAndWaitInternal(ctx, cmd_buf);
	}

	TextureInternal::~TextureInternal()
	{
#ifdef USE_IMGUI
		if (imgui_set != VK_NULL_HANDLE)
		{
			ImGui_ImplVulkan_RemoveTexture(imgui_set);
		}
#endif
	};
}