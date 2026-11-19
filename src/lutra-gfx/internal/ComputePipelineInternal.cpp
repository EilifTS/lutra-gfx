#include "ComputePipelineInternal.h"
#include "PipelineLayoutInternal.h"

namespace lgx
{
	ComputePipelineInternal::ComputePipelineInternal(vk::Device dev, const ComputePipelineInfo& info)
	{
		push_constant_size = info.push_constant_size;
		cs_module = CreateShaderModule(dev, info.cs_name);

		PipelineLayoutResult layout_result = CreatePipelineLayout(dev, info.bindings, info.push_constant_size, vk::ShaderStageFlagBits::eCompute);
		desc_layout = std::move(layout_result.desc_layout);
		layout = std::move(layout_result.layout);
		samplers = std::move(layout_result.samplers);

		const vk::PipelineShaderStageCreateInfo stage_info{
			.stage = vk::ShaderStageFlagBits::eCompute,
			.module = *cs_module,
			.pName = "CS",
		};

		const vk::ComputePipelineCreateInfo pipeline_info{
			.stage = stage_info,
			.layout = *layout,
		};

		auto [res, ppl] = dev.createComputePipelineUnique(VK_NULL_HANDLE, pipeline_info);
		assert(res == vk::Result::eSuccess);
		pipeline = std::move(ppl);
	}
}
