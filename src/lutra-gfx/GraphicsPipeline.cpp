#include <lutra-gfx/GraphicsPipeline.h>
#include "internal/GraphicsContextInternal.h"
#include "internal/GraphicsPipelineInternal.h"

namespace lgx
{
	GraphicsPipeline::GraphicsPipeline() {}
	GraphicsPipeline::GraphicsPipeline(GraphicsContext& ctx, const GraphicsPipelineInfo& info)
	{
		internal = std::make_unique<GraphicsPipelineInternal>(*ctx.internal->device, info);
	}

	GraphicsPipeline::~GraphicsPipeline() {}

	GraphicsPipeline::GraphicsPipeline(GraphicsPipeline&&) = default;
	GraphicsPipeline& GraphicsPipeline::operator=(GraphicsPipeline&&) = default;
}