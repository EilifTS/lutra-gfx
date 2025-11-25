#pragma once
#include "VulkanHPP.h"
#include <efvk/math/IntDefs.h>

namespace efvk
{
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
		};

		void AddStorageBuffer(u32 binding, Binding::Stage stage)
		{
			bindings.push_back({ binding, Binding::Type::StorageBuffer, 1, stage });
		}

		std::vector<Binding> bindings{};
	};

	class GraphicsPipeline
	{
	public:
		GraphicsPipeline() {};
		GraphicsPipeline(vk::Device dev, const GraphicsPipelineInfo& info);

		void Compile(vk::Device dev);

		vk::DescriptorSetLayout GetDescriptorSetLayout() { return *desc_layout; }
		vk::PipelineLayout GetPipelineLayout() { return *layout; }
		vk::Pipeline GetPipeline() { return *pipeline; }

	private:
		vk::UniqueShaderModule vs_module{};
		vk::UniqueShaderModule ps_module{};
		vk::UniqueDescriptorSetLayout desc_layout{};
		vk::UniquePipelineLayout layout{};
		vk::UniquePipeline pipeline{};
	};
}