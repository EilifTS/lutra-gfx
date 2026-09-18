#pragma once
#include <memory>
#include "GraphicsContext.h"
#include "CommonStructs.h"

namespace lgx
{
	class ComputePipelineInternal;

	class ComputePipeline
	{
	public:
		ComputePipeline();
		ComputePipeline(GraphicsContext& ctx, const ComputePipelineInfo& info);
		~ComputePipeline();

		ComputePipeline(const ComputePipeline&) = delete;
		ComputePipeline& operator=(const ComputePipeline&) = delete;

		ComputePipeline(ComputePipeline&&);
		ComputePipeline& operator=(ComputePipeline&&);

		std::unique_ptr<ComputePipelineInternal> internal{};
	};
}
