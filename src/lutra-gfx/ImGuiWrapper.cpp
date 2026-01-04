#ifdef USE_IMGUI

#include <lutra-gfx/ImGuiWrapper.h>
#include <lutra-gfx/GraphicsContext.h>
#include <lutra-gfx/FrameManager.h>
#include <lutra-gfx/Window.h>
#include <lutra-gfx/Texture.h>
#include "internal/FrameManagerInternal.h"
#include "internal/GraphicsContextInternal.h"
#include "internal/VulkanHPP.h"

#include <imgui.h>

#define IMGUI_IMPL_VULKAN_USE_VOLK
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>


namespace lgx
{
	static constexpr u32 max_extra_textures = 256;
	static vk::UniqueDescriptorPool descriptor_pool{};

	static void InitVulkanResources(GraphicsContextInternal& ctx)
	{
		/* Create descriptor pool */
		const vk::DescriptorPoolSize pool_sizes[] = {
			{ vk::DescriptorType::eCombinedImageSampler, 1 + max_extra_textures }
		};

		const vk::DescriptorPoolCreateInfo desc_pool_info{
			.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
			.maxSets = 1 + max_extra_textures,
			.poolSizeCount = 1,
			.pPoolSizes = pool_sizes,
		};
		descriptor_pool = ctx.device->createDescriptorPoolUnique(desc_pool_info);
	}

	static void CleanupVulkanResources()
	{
		descriptor_pool.reset(VK_NULL_HANDLE);
	}

	static void CheckVKResult(VkResult err)
	{
		if (err == VK_SUCCESS)
			return;
		fprintf(stderr, "[vulkan] Error: VkResult = %s\n", vk::to_string(static_cast<vk::Result>(err)).c_str());
		if (err < 0)
			abort();
	}

	void ImGuiWrapper::Initialize(Window& window, GraphicsContext& ctx)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		InitVulkanResources(*ctx.internal);

		ImGui_ImplGlfw_InitForVulkan(reinterpret_cast<GLFWwindow*>(window.GetHandle()), true);

		const vk::Format attachment_format = vk::Format::eR8G8B8A8Unorm;
		const vk::PipelineRenderingCreateInfo pipeline_rendering_info{
			.colorAttachmentCount = 1,
			.pColorAttachmentFormats = &attachment_format,
		};

		ImGui_ImplVulkan_InitInfo vulkan_info = {};
		vulkan_info.Instance = *ctx.internal->instance;
		vulkan_info.PhysicalDevice = ctx.internal->physical_device;
		vulkan_info.Device = *ctx.internal->device;
		vulkan_info.QueueFamily = ctx.internal->queue_family_index;
		vulkan_info.Queue = ctx.internal->queue;
		vulkan_info.PipelineCache = VK_NULL_HANDLE;
		vulkan_info.DescriptorPool = *descriptor_pool;
		vulkan_info.MinImageCount = 3;
		vulkan_info.ImageCount = 3;
		vulkan_info.Allocator = nullptr;
		vulkan_info.RenderPass = VK_NULL_HANDLE;
		vulkan_info.Subpass = 0;
		vulkan_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		vulkan_info.CheckVkResultFn = CheckVKResult;
		vulkan_info.UseDynamicRendering = true;
		vulkan_info.PipelineRenderingCreateInfo = pipeline_rendering_info;
		const bool result = ImGui_ImplVulkan_Init(&vulkan_info);
		assert(result);
	}

	void ImGuiWrapper::StartFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiWrapper::EndFrame(FrameManager& frame_manager)
	{
		ImGui::Render();
		ImDrawData* draw_data = ImGui::GetDrawData();

		vk::CommandBuffer cmd_buf = *frame_manager.internal->GetCurrentCommandBuffer().internal->cmd_buf;

		const vk::RenderingAttachmentInfo render_attachment_info{
			.imageView = frame_manager.internal->GetCurrentImageView(),
			.imageLayout = vk::ImageLayout::eGeneral,
			.loadOp = vk::AttachmentLoadOp::eLoad,
			.storeOp = vk::AttachmentStoreOp::eStore,
		};

		const vk::RenderingInfo rendering_info{
			.renderArea = { 0, 0, frame_manager.internal->window_width, frame_manager.internal->window_height },
			.layerCount = 1,
			.colorAttachmentCount = 1,
			.pColorAttachments = &render_attachment_info,
		};

		cmd_buf.beginRendering(rendering_info);
		ImGui_ImplVulkan_RenderDrawData(draw_data, cmd_buf);
		cmd_buf.endRendering();
	}

	void ImGuiWrapper::ShutDown()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		CleanupVulkanResources();
	}


	void ImGuiWrapper::Image(const lgx::Texture& texture, float width, float height)
	{
		ImGui::Image(texture.GetImGuiID(), ImVec2(width, height));
	}
	void ImGuiWrapper::Image(const lgx::Texture& texture, float src_x, float src_y, float src_w, float src_h, float width, float height)
	{
		ImGui::Image(texture.GetImGuiID(), ImVec2(width, height), ImVec2(src_x, src_y), ImVec2(src_x + src_w, src_y + src_h));
	}
	bool ImGuiWrapper::ImageButton(const char* id, const lgx::Texture& texture, float src_x, float src_y, float src_w, float src_h, float width, float height)
	{
		return ImGui::ImageButton(id, texture.GetImGuiID(), ImVec2(width, height), ImVec2(src_x, src_y), ImVec2(src_x + src_w, src_y + src_h));
	}
}

#endif