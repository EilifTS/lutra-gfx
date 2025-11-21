#pragma once
#include "VulkanHPP.h"

namespace efvk
{
	class GraphicsPipeline
	{
	public:
		GraphicsPipeline() {};
		GraphicsPipeline(vk::Device dev, const char* vs_name, const char* ps_name);

		void Compile(vk::Device dev);

		vk::DescriptorSetLayout GetDescriptorSetLayout() { return *desc_layout; }
		vk::PipelineLayout GetPipelineLayout() { return *layout; }
		vk::Pipeline GetPipeline() { return *pipeline; }

	private:
		vk::UniqueShaderModule vs_module{};
		vk::UniqueShaderModule ps_module{};
		vk::UniqueDescriptorSetLayout desc_layout{};
		vk::UniquePipelineLayout layout{};
		vk::UniquePipeline pipeline{};
	};
}