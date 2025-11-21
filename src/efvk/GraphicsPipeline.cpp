#include "GraphicsPipeline.h"
#include <vector>
#include <fstream>

namespace efvk
{
	static std::vector<uint32_t> load_spriv_data(const char* path)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		assert(file.is_open());
		
		const int file_size = static_cast<int>(file.tellg());
		assert(file_size % 4 == 0);

		std::vector<uint32_t> data(file_size / 4);
		file.seekg(0);
		file.read(reinterpret_cast<char*>(data.data()), file_size);

		return data;
	}

	static vk::UniqueShaderModule create_shader_module(vk::Device dev, const char* path)
	{
		const std::vector<uint32_t> data = load_spriv_data(path);
		vk::ShaderModuleCreateInfo vs_module_info{
			.codeSize = data.size() * 4,
			.pCode = data.data(),
		};
		return dev.createShaderModuleUnique(vs_module_info);
	}

	GraphicsPipeline::GraphicsPipeline(vk::Device dev, const char* vs_name, const char* ps_name)
	{
		vs_module = create_shader_module(dev, vs_name);
		ps_module = create_shader_module(dev, ps_name);
	}

	void GraphicsPipeline::Compile(vk::Device dev)
	{
		/* Create descriptor set layout */
		const vk::DescriptorSetLayoutBinding desc_layout_binding{
			.binding = 0,
			.descriptorType = vk::DescriptorType::eStorageBuffer,
			.descriptorCount = 1,
			.stageFlags = vk::ShaderStageFlagBits::eVertex,
		};

		const vk::DescriptorSetLayoutCreateInfo desc_layout_info{
			.bindingCount = 1,
			.pBindings = &desc_layout_binding,
		};

		desc_layout = dev.createDescriptorSetLayoutUnique(desc_layout_info);

		/* Create pipeline layout */
		const vk::PipelineLayoutCreateInfo layout_info{
			.setLayoutCount = 1,
			.pSetLayouts = &desc_layout.get(),
		};
		layout = dev.createPipelineLayoutUnique(layout_info);

		const vk::Format color_attachment_format = vk::Format::eR8G8B8A8Unorm;
		const vk::PipelineRenderingCreateInfo rendering_info{
			.colorAttachmentCount = 1,
			.pColorAttachmentFormats = &color_attachment_format,
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
			.topology = vk::PrimitiveTopology::eTriangleList,
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
			.depthTestEnable = false,
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