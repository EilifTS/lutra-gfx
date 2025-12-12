#pragma once
#include <vector>
#include <efvk/math/IntDefs.h>
#include "GraphicsContext.h"

namespace efvk
{
	class GraphicsPipelineInternal;

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
		GraphicsPipeline();
		GraphicsPipeline(GraphicsContext& ctx, const GraphicsPipelineInfo& info);
		~GraphicsPipeline();

		GraphicsPipeline(const GraphicsPipeline&) = delete;
		GraphicsPipeline(GraphicsPipeline&&);
		GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
		GraphicsPipeline& operator=(GraphicsPipeline&&);

		std::unique_ptr<GraphicsPipelineInternal> internal{};
	};
}