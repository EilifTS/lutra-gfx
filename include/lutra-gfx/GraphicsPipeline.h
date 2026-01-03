#pragma once
#include <vector>
#include "GraphicsContext.h"
#include "CommonStructs.h"

namespace efvk
{
	class GraphicsPipelineInternal;

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