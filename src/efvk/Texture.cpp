#include <efvk/Texture.h>

#include "VulkanHPP.h"
#include "GraphicsContextImpl.h"
#include "TextureImpl.h"
#include "CommandBuffer.h"

#ifdef USE_IMGUI
#include <backends/imgui_impl_vulkan.h>
#endif

namespace efvk
{
	Texture::Texture(){}

	Texture::Texture(GraphicsContext& ctx, const Image& image)
	{
		pimpl = std::make_unique<Texture::Impl>();

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

		pimpl->width = image.Width();
		pimpl->height = image.Height();
		pimpl->format = vk::Format::eR8G8B8A8Unorm;
		pimpl->dev = *ctx.pimpl->device;
		pimpl->image = VMACreateImage(ctx.pimpl->vma_allocator, &image_info, &vma_info, nullptr);

		/* Create view */
		const vk::ImageViewCreateInfo view_info{
			.image = pimpl->image.GetImage(),
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

		pimpl->view = ctx.pimpl->device->createImageViewUnique(view_info);

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
		pimpl->imgui_sampler = ctx.pimpl->device->createSamplerUnique(sampler_info);

		pimpl->imgui_set = ImGui_ImplVulkan_AddTexture(*pimpl->imgui_sampler, *pimpl->view, VK_IMAGE_LAYOUT_GENERAL);
#endif

		/* Initialize content */
		CommandBuffer cmd_buf(*ctx.pimpl);
		change_layout(cmd_buf.cmd_buf.get(), pimpl->image.GetImage(), vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
		cmd_buf.ScheduleUpload(image.GetDataPtr(), *this);
		SubmitAndWait(*ctx.pimpl, cmd_buf);
	}

	Texture::~Texture()
	{
#ifdef USE_IMGUI
		if (pimpl != nullptr && pimpl->imgui_set != VK_NULL_HANDLE)
		{
			ImGui_ImplVulkan_RemoveTexture(pimpl->imgui_set);
		}
#endif
	};
	Texture::Texture(Texture&&) = default;
	Texture& Texture::operator=(Texture&&) = default;

	u32 Texture::Width() const {
		return pimpl->width;
	}

	u32 Texture::Height() const {
		return pimpl->height;
	}

#ifdef USE_IMGUI
	void* Texture::GetImGuiID() const {
		return static_cast<void*>(pimpl->imgui_set);
	}
#endif
}