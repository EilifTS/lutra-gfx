#include "RenderTargetInternal.h"
#include "CommandBufferInternal.h"

#ifdef USE_IMGUI
#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#endif

namespace lgx
{
	RenderTargetInternal::RenderTargetInternal(GraphicsContextInternal& ctx, u32 width, u32 height)
	{
		VmaAllocationCreateInfo vma_info{};
		vma_info.usage = VMA_MEMORY_USAGE_AUTO;

		const vk::ImageCreateInfo image_info{
			.imageType = vk::ImageType::e2D,
			.format = vk::Format::eR8G8B8A8Unorm,
			.extent = { width, height, 1 },
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = vk::SampleCountFlagBits::e1,
			.tiling = vk::ImageTiling::eOptimal,
			.usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst,
			.sharingMode = vk::SharingMode::eExclusive,
			.initialLayout = vk::ImageLayout::eUndefined,
		};

		this->width = width;
		this->height = height;
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

		view = VkCheck(ctx.device->createImageViewUnique(view_info));

		/* Initialize ImGui resources (so a render target can be previewed like any texture),
		   but only if ImGuiWrapper::Initialize() has actually run - otherwise ImGui's Vulkan
		   backend isn't set up and this would crash. */
#ifdef USE_IMGUI
		if (ImGui::GetCurrentContext() != nullptr)
		{
			const vk::SamplerCreateInfo sampler_info{
				.magFilter = vk::Filter::eNearest,
				.minFilter = vk::Filter::eNearest,
				.addressModeU = vk::SamplerAddressMode::eClampToEdge,
				.addressModeV = vk::SamplerAddressMode::eClampToEdge,
				.addressModeW = vk::SamplerAddressMode::eClampToEdge,
				.maxLod = vk::LodClampNone,
			};
			imgui_sampler = VkCheck(ctx.device->createSamplerUnique(sampler_info));

			imgui_set = ImGui_ImplVulkan_AddTexture(*imgui_sampler, *view, VK_IMAGE_LAYOUT_GENERAL);
		}
#endif

		/* Transition to General up front; the caller will render into it before ever sampling it */
		CommandBufferInternal cmd_buf(ctx);
		change_layout(cmd_buf.cmd_buf.get(), vma_image.GetImage(), vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral, vk::ImageAspectFlagBits::eColor);
		SubmitAndWaitInternal(ctx, cmd_buf);
	}

	RenderTargetInternal::~RenderTargetInternal()
	{
#ifdef USE_IMGUI
		if (imgui_set != VK_NULL_HANDLE)
		{
			ImGui_ImplVulkan_RemoveTexture(imgui_set);
		}
#endif
	};
}
