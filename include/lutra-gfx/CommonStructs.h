#pragma once
#include <inttypes.h>
#include <vector>

using u32 = uint32_t;
using u64 = uint64_t;

namespace lgx
{
	struct MousePosition
	{
		int x{};
		int y{};
	};

	using TextureView = void*;

	enum class PrimitiveTopology
	{
		LineList,
		TriangleList,
	};

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

	/* What a resource was/will be used for, for CommandBuffer::Barrier. Every image in this
	   library stays in VK_IMAGE_LAYOUT_GENERAL permanently, so a barrier here only ever needs
	   to synchronize stage/access - there's no layout tracking to do. */
	enum class ResourceUsage
	{
		ColorAttachment,
		DepthAttachment,
		ShaderRead,
		ComputeWrite,
		TransferSrc,
		TransferDst,
	};

	/* A single descriptor set binding, shared by GraphicsPipelineInfo and ComputePipelineInfo. */
	struct PipelineBinding
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
			Compute,
		};

		u32 binding_index{};
		Type type{};
		u32 count{};
		Stage stage{};
		SamplerType sampler_type{};
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

		PrimitiveTopology topology{ PrimitiveTopology::TriangleList };

		/* Kept as a nested alias so existing code referring to GraphicsPipelineInfo::Binding
		   keeps compiling unchanged. */
		using Binding = PipelineBinding;

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
		void AddStorageImage(u32 binding, Binding::Stage stage)
		{
			bindings.push_back({ binding, Binding::Type::StorageImage, 1, stage, SamplerType::None });
		}
		void AddStorageBuffer(u32 binding, Binding::Stage stage)
		{
			bindings.push_back({ binding, Binding::Type::StorageBuffer, 1, stage, SamplerType::None });
		}
		void AddUniformBuffer(u32 binding, Binding::Stage stage)
		{
			bindings.push_back({ binding, Binding::Type::UniformBuffer, 1, stage, SamplerType::None });
		}

		std::vector<Binding> bindings{};
	};

	/* Every binding is implicitly Stage::Compute, so unlike GraphicsPipelineInfo there's no
	   stage parameter to pass. */
	struct ComputePipelineInfo
	{
		const char* cs_name{};

		using Binding = PipelineBinding;

		void AddImmutableSampler(u32 binding, SamplerType type)
		{
			bindings.push_back({ binding, Binding::Type::Sampler, 1, Binding::Stage::Compute, type });
		}
		void AddTexture(u32 binding)
		{
			bindings.push_back({ binding, Binding::Type::SampledImage, 1, Binding::Stage::Compute, SamplerType::None });
		}
		void AddTextures(u32 binding, u32 count)
		{
			bindings.push_back({ binding, Binding::Type::SampledImage, count, Binding::Stage::Compute, SamplerType::None });
		}
		void AddStorageImage(u32 binding)
		{
			bindings.push_back({ binding, Binding::Type::StorageImage, 1, Binding::Stage::Compute, SamplerType::None });
		}
		void AddStorageBuffer(u32 binding)
		{
			bindings.push_back({ binding, Binding::Type::StorageBuffer, 1, Binding::Stage::Compute, SamplerType::None });
		}
		void AddUniformBuffer(u32 binding)
		{
			bindings.push_back({ binding, Binding::Type::UniformBuffer, 1, Binding::Stage::Compute, SamplerType::None });
		}

		std::vector<Binding> bindings{};
	};
}