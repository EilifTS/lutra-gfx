#pragma once
#include <inttypes.h>
#include <vector>

using u32 = uint32_t;
using u64 = uint64_t;

namespace efvk
{
	struct MousePosition
	{
		int x{};
		int y{};
	};

	using TextureView = void*;

	enum class DepthStencilFormat
	{
		D32,
	};

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

		struct DSInfo
		{
			bool depth_enabled{ false };
			DepthStencilFormat ds_format{};
		};
		DSInfo ds_info{};

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
}