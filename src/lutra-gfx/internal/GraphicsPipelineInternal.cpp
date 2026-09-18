#include "GraphicsPipelineInternal.h"
#include "PipelineLayoutInternal.h"
#include "CommonHelpers.h"
#include <vector>

namespace lgx
{
	GraphicsPipelineInternal::GraphicsPipelineInternal(vk::Device dev, const GraphicsPipelineInfo& info)
	{
		vs_module = CreateShaderModule(dev, info.vs_name);
		ps_module = CreateShaderModule(dev, info.ps_name);

		PipelineLayoutResult layout_result = CreatePipelineLayout(dev, info.bindings);
		desc_layout = std::move(layout_result.desc_layout);
		layout = std::move(layout_result.layout);
		samplers = std::move(layout_result.samplers);

		const vk::Format color_attachment_format = vk::Format::eR8G8B8A8Unorm;
		const vk::PipelineRenderingCreateInfo rendering_info{
			.colorAttachmentCount = 1,
			.pColorAttachmentFormats = &color_attachment_format,
			.depthAttachmentFormat = info.ds_info.depth_enabled ? convert_ds_format(info.ds_info.ds_format) : vk::Format::eUndefined
		};

		const vk::PipelineShaderStageCreateInfo stage_infos[] = {
			{
				.stage = vk::ShaderStageFlagBits::eVertex,
				.module = *vs_module,
				.pName = "VS",
			},
			{
				.stage = vk::ShaderStageFlagBits::eFragment,
				.module = *ps_module,
				.pName = "PS",
			},
		};

		const vk::PipelineVertexInputStateCreateInfo vertex_input_info{};

		const vk::PipelineInputAssemblyStateCreateInfo input_assembly_info{
			.topology = info.topology == PrimitiveTopology::TriangleList ? vk::PrimitiveTopology::eTriangleList : vk::PrimitiveTopology::eLineList,
			.primitiveRestartEnable = false
		};

		const vk::PipelineViewportStateCreateInfo viewport_info{
			.viewportCount = 1,
			.scissorCount = 1,
		};

		const vk::PipelineRasterizationStateCreateInfo raster_info{
			.lineWidth = 1.0f,
		};

		const vk::PipelineMultisampleStateCreateInfo multisample_state{
			.rasterizationSamples = vk::SampleCountFlagBits::e1,
		};

		const vk::PipelineDepthStencilStateCreateInfo depth_stencil_info{
			.depthTestEnable = info.ds_info.depth_enabled,
			.depthWriteEnable = info.ds_info.depth_enabled,
			.depthCompareOp = vk::CompareOp::eGreaterOrEqual,
			.stencilTestEnable = false,
		};

		const vk::PipelineColorBlendAttachmentState blend_attachment_info{
			.blendEnable = false,
			.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
		};

		const vk::PipelineColorBlendStateCreateInfo blend_info{
			.attachmentCount = 1,
			.pAttachments = &blend_attachment_info,
		};

		const vk::DynamicState dynamic_states[] = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor,
		};

		const vk::PipelineDynamicStateCreateInfo dynamic_info{
			.dynamicStateCount = sizeof(dynamic_states) / sizeof(vk::DynamicState),
			.pDynamicStates = dynamic_states
		};

		const vk::GraphicsPipelineCreateInfo pipeline_info{
			.pNext = &rendering_info,
			.stageCount = 2,
			.pStages = stage_infos,
			.pVertexInputState = &vertex_input_info,
			.pInputAssemblyState = &input_assembly_info,
			.pViewportState = &viewport_info,
			.pRasterizationState = &raster_info,
			.pMultisampleState = &multisample_state,
			.pDepthStencilState = &depth_stencil_info,
			.pColorBlendState = &blend_info,
			.pDynamicState = &dynamic_info,
			.layout = *layout,
		};
		auto [res, ppl] = dev.createGraphicsPipelineUnique(VK_NULL_HANDLE, pipeline_info);
		assert(res == vk::Result::eSuccess);
		pipeline = std::move(ppl);
	}

}