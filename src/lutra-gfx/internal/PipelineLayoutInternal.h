#pragma once
#include <lutra-gfx/CommonStructs.h>
#include "VulkanHPP.h"
#include <vector>

namespace lgx
{
	/* Shared by GraphicsPipelineInternal and ComputePipelineInternal - both need identical
	   shader module loading and descriptor set/pipeline layout construction from a list of
	   PipelineBindings; only the actual vk::Pipeline creation differs between them. */

	vk::UniqueShaderModule CreateShaderModule(vk::Device dev, const char* path);

	struct PipelineLayoutResult
	{
		vk::UniqueDescriptorSetLayout desc_layout{};
		vk::UniquePipelineLayout layout{};

		/* Immutable samplers referenced by desc_layout - must be kept alive for as long as
		   desc_layout (and any pipeline/descriptor sets built from it) are in use. */
		std::vector<vk::UniqueSampler> samplers{};
	};

	PipelineLayoutResult CreatePipelineLayout(vk::Device dev, const std::vector<PipelineBinding>& bindings);
}
