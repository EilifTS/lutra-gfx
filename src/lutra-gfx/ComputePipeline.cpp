#include <lutra-gfx/ComputePipeline.h>
#include "internal/GraphicsContextInternal.h"
#include "internal/ComputePipelineInternal.h"

namespace lgx
{
	ComputePipeline::ComputePipeline() {}
	ComputePipeline::ComputePipeline(GraphicsContext& ctx, const ComputePipelineInfo& info)
	{
		internal = std::make_unique<ComputePipelineInternal>(*ctx.internal->device, info);
	}

	ComputePipeline::~ComputePipeline() {}

	ComputePipeline::ComputePipeline(ComputePipeline&&) = default;
	ComputePipeline& ComputePipeline::operator=(ComputePipeline&&) = default;
}
