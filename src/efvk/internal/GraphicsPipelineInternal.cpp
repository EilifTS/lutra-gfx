#include "GraphicsPipelineInternal.h"
#include "CommonHelpers.h"
#include <vector>
#include <fstream>

namespace efvk
{
	static vk::DescriptorType convertDescriptorType(GraphicsPipelineInfo::Binding::Type type)
	{
		switch (type)
		{
		case GraphicsPipelineInfo::Binding::Type::Sampler: return vk::DescriptorType::eSampler;
		case GraphicsPipelineInfo::Binding::Type::SampledImage: return vk::DescriptorType::eSampledImage;
		case GraphicsPipelineInfo::Binding::Type::StorageImage: return vk::DescriptorType::eStorageImage;
		case GraphicsPipelineInfo::Binding::Type::UniformBuffer: return vk::DescriptorType::eUniformBuffer;
		case GraphicsPipelineInfo::Binding::Type::StorageBuffer: return vk::DescriptorType::eStorageBuffer;
		}
		assert(0);
		return vk::DescriptorType::eSampler;
	}

	static vk::ShaderStageFlagBits convertDescriptorStage(GraphicsPipelineInfo::Binding::Stage stage)
	{
		switch (stage)
		{
		case GraphicsPipelineInfo::Binding::Stage::Vertex: return vk::ShaderStageFlagBits::eVertex;
		case GraphicsPipelineInfo::Binding::Stage::Fragment: return vk::ShaderStageFlagBits::eFragment;
		}
		assert(0);
		return vk::ShaderStageFlagBits::eVertex;
	}

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

	static vk::Filter getFilter(SamplerType type)
	{
		switch (type)
		{
		case SamplerType::LinearClamp:
		case SamplerType::LinearWrap:
			return vk::Filter::eLinear;
		case SamplerType::PointClamp:
		case SamplerType::PointWrap:
			return vk::Filter::eNearest;
		default:
			assert(0);
		}
		return vk::Filter::eLinear;
	}

	static vk::SamplerAddressMode getAddressMode(SamplerType type)
	{
		switch (type)
		{
		case SamplerType::LinearClamp:
		case SamplerType::PointClamp:
			return vk::SamplerAddressMode::eClampToEdge;
		case SamplerType::LinearWrap:
		case SamplerType::PointWrap:
			return vk::SamplerAddressMode::eRepeat;
		default:
			assert(0);
		}
		return vk::SamplerAddressMode::eClampToEdge;
	}

	static vk::UniqueSampler create_sampler(vk::Device dev, SamplerType type)
	{
		const vk::Filter filter = getFilter(type);
		const vk::SamplerAddressMode address_mode = getAddressMode(type);

		const vk::SamplerCreateInfo info{
			.magFilter = filter,
			.minFilter = filter,
			.addressModeU = address_mode,
			.addressModeV = address_mode,
			.addressModeW = address_mode,
			.maxLod = vk::LodClampNone,
		};

		return dev.createSamplerUnique(info);
	}

	GraphicsPipelineInternal::GraphicsPipelineInternal(vk::Device dev, const GraphicsPipelineInfo& info)
	{
		vs_module = create_shader_module(dev, info.vs_name);
		ps_module = create_shader_module(dev, info.ps_name);

		/* Create descriptor set layout */
		std::vector<vk::DescriptorSetLayoutBinding> desc_layout_bindings{};
		for (u32 i = 0; i < static_cast<u32>(info.bindings.size()); i++)
		{
			const auto& b = info.bindings[i];
			if (b.type == GraphicsPipelineInfo::Binding::Type::Sampler)
			{
				samplers.push_back(create_sampler(dev, b.sampler_type));

				desc_layout_bindings.push_back({
					.binding = b.binding_index,
					.descriptorType = convertDescriptorType(b.type),
					.descriptorCount = b.count,
					.stageFlags = convertDescriptorStage(b.stage),
					.pImmutableSamplers = &samplers.back().get()
				});
			}
			else
			{
				desc_layout_bindings.push_back({
					.binding = b.binding_index,
					.descriptorType = convertDescriptorType(b.type),
					.descriptorCount = b.count,
					.stageFlags = convertDescriptorStage(b.stage),
				});
			}
			
		}

		const vk::DescriptorSetLayoutCreateInfo desc_layout_info{
			.bindingCount = static_cast<u32>(desc_layout_bindings.size()),
			.pBindings = desc_layout_bindings.data(),
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