#pragma once
#include <lutra-gfx/CommonStructs.h>
#include "VulkanHPP.h"

namespace lgx
{
	class ComputePipelineInternal
	{
	public:
		ComputePipelineInternal(vk::Device dev, const ComputePipelineInfo& info);

		vk::DescriptorSetLayout GetDescriptorSetLayout() { return *desc_layout; }
		vk::PipelineLayout GetPipelineLayout() { return *layout; }
		vk::Pipeline GetPipeline() { return *pipeline; }

	private:
		vk::UniqueShaderModule cs_module{};
		vk::UniqueDescriptorSetLayout desc_layout{};
		vk::UniquePipelineLayout layout{};
		vk::UniquePipeline pipeline{};

		std::vector<vk::UniqueSampler> samplers{};
	};
}
