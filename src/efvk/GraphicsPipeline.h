#pragma once
#include "VulkanHPP.h"
#include <efvk/math/IntDefs.h>

namespace efvk
{
	enum class SamplerType
	{
		None,
		LinearClamp,
		LinearWrap,
		PointClamp,
		PointWrap
	};

	struct GraphicsPipelineInfo
	{
		const char* vs_name{};
		const char* ps_name{};

		struct Binding
		{
			enum class Type
			{
				Sampler,
				SampledImage,
				StorageImage,
				UniformBuffer,
				StorageBuffer,
			};

			enum class Stage
			{
				Vertex,
				Fragment,
			};

			u32 binding_index{};
			Type type{};
			u32 count{};
			Stage stage{};
			SamplerType sampler_type{};
		};

		void AddImmutableSampler(u32 binding, SamplerType type, Binding::Stage stage)
		{
			bindings.push_back({ binding, Binding::Type::Sampler, 1, stage, type });
		}
		void AddTexture(u32 binding, Binding::Stage stage)
		{
			bindings.push_back({ binding, Binding::Type::SampledImage, 1, stage, SamplerType::None });
		}
		void AddTextures(u32 binding, u32 count, Binding::Stage stage)
		{
			bindings.push_back({ binding, Binding::Type::SampledImage, count, stage, SamplerType::None });
		}
		void AddStorageBuffer(u32 binding, Binding::Stage stage)
		{
			bindings.push_back({ binding, Binding::Type::StorageBuffer, 1, stage, SamplerType::None });
		}

		std::vector<Binding> bindings{};
	};

	class GraphicsPipeline
	{
	public:
		GraphicsPipeline() {};
		GraphicsPipeline(vk::Device dev, const GraphicsPipelineInfo& info);

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