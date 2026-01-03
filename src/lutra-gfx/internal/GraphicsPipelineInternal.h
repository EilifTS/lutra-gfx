#pragma once
#include <lutra-gfx/CommonStructs.h>
#include <lutra-gfx/GraphicsPipeline.h>
#include "VulkanHPP.h"

namespace lgx
{
	class GraphicsPipelineInternal
	{
	public:
		GraphicsPipelineInternal(vk::Device dev, const GraphicsPipelineInfo& info);

		vk::DescriptorSetLayout GetDescriptorSetLayout() { return *desc_layout; }
		vk::PipelineLayout GetPipelineLayout() { return *layout; }
		vk::Pipeline GetPipeline() { return *pipeline; }

	private:
		vk::UniqueShaderModule vs_module{};
		vk::UniqueShaderModule ps_module{};
		vk::UniqueDescriptorSetLayout desc_layout{};
		vk::UniquePipelineLayout layout{};
		vk::UniquePipeline pipeline{};
		
		std::vector<vk::UniqueSampler> samplers{};
	};
}