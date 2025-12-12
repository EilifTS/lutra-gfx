#pragma once
#include <efvk/math/IntDefs.h>
#include <efvk/GraphicsPipeline.h>
#include "VulkanHPP.h"

namespace efvk
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