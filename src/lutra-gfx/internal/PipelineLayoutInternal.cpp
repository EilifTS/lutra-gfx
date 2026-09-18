#include "PipelineLayoutInternal.h"
#include <fstream>

namespace lgx
{
	static std::vector<uint32_t> load_spirv_data(const char* path)
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

	vk::UniqueShaderModule CreateShaderModule(vk::Device dev, const char* path)
	{
		const std::vector<uint32_t> data = load_spirv_data(path);
		const vk::ShaderModuleCreateInfo module_info{
			.codeSize = data.size() * 4,
			.pCode = data.data(),
		};
		return VkCheck(dev.createShaderModuleUnique(module_info));
	}

	static vk::DescriptorType convertDescriptorType(PipelineBinding::Type type)
	{
		switch (type)
		{
		case PipelineBinding::Type::Sampler: return vk::DescriptorType::eSampler;
		case PipelineBinding::Type::SampledImage: return vk::DescriptorType::eSampledImage;
		case PipelineBinding::Type::StorageImage: return vk::DescriptorType::eStorageImage;
		case PipelineBinding::Type::UniformBuffer: return vk::DescriptorType::eUniformBuffer;
		case PipelineBinding::Type::StorageBuffer: return vk::DescriptorType::eStorageBuffer;
		}
		assert(0);
		return vk::DescriptorType::eSampler;
	}

	static vk::ShaderStageFlagBits convertDescriptorStage(PipelineBinding::Stage stage)
	{
		switch (stage)
		{
		case PipelineBinding::Stage::Vertex: return vk::ShaderStageFlagBits::eVertex;
		case PipelineBinding::Stage::Fragment: return vk::ShaderStageFlagBits::eFragment;
		case PipelineBinding::Stage::Compute: return vk::ShaderStageFlagBits::eCompute;
		}
		assert(0);
		return vk::ShaderStageFlagBits::eVertex;
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

		return VkCheck(dev.createSamplerUnique(info));
	}

	PipelineLayoutResult CreatePipelineLayout(vk::Device dev, const std::vector<PipelineBinding>& bindings)
	{
		PipelineLayoutResult result{};

		std::vector<vk::DescriptorSetLayoutBinding> desc_layout_bindings{};

		/* Reserve up front: bindings below take the address of a `samplers` element, which would
		   dangle if a later push_back reallocated the vector. */
		result.samplers.reserve(bindings.size());

		for (u32 i = 0; i < static_cast<u32>(bindings.size()); i++)
		{
			const PipelineBinding& b = bindings[i];
			if (b.type == PipelineBinding::Type::Sampler)
			{
				result.samplers.push_back(create_sampler(dev, b.sampler_type));

				desc_layout_bindings.push_back({
					.binding = b.binding_index,
					.descriptorType = convertDescriptorType(b.type),
					.descriptorCount = b.count,
					.stageFlags = convertDescriptorStage(b.stage),
					.pImmutableSamplers = &result.samplers.back().get()
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
		result.desc_layout = VkCheck(dev.createDescriptorSetLayoutUnique(desc_layout_info));

		const vk::PipelineLayoutCreateInfo layout_info{
			.setLayoutCount = 1,
			.pSetLayouts = &result.desc_layout.get(),
		};
		result.layout = VkCheck(dev.createPipelineLayoutUnique(layout_info));

		return result;
	}
}
