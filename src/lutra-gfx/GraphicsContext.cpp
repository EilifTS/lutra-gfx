#include <lutra-gfx/GraphicsContext.h>

#include "internal/GraphicsContextInternal.h"

namespace lgx
{
	GraphicsContext::GraphicsContext(const char* app_name)
		: GraphicsContext(app_name, GraphicsFeatures{})
	{
	}

	GraphicsContext::GraphicsContext(const char* app_name, const Window& window)
		: GraphicsContext(app_name, window, GraphicsFeatures{})
	{
	}

	GraphicsContext::GraphicsContext(const char* app_name, const GraphicsFeatures& requested_features)
	{
		internal = std::make_unique<GraphicsContextInternal>(app_name, requested_features);
	}

	GraphicsContext::GraphicsContext(const char* app_name, const Window& window, const GraphicsFeatures& requested_features)
	{
		internal = std::make_unique<GraphicsContextInternal>(app_name, window, requested_features);
	}

	GraphicsContext::~GraphicsContext() {}

	GraphicsContext::GraphicsContext(GraphicsContext&&) = default;
	GraphicsContext& GraphicsContext::operator=(GraphicsContext&&) = default;

	ColorFormat GraphicsContext::SwapchainFormat() const
	{
		return internal->swapchain_color_format;
	}

	const GraphicsFeatures& GraphicsContext::EnabledFeatures() const
	{
		return internal->enabled_features;
	}

	u32 GraphicsContext::MinUniformBufferOffsetAlignment() const
	{
		return static_cast<u32>(internal->limits.minUniformBufferOffsetAlignment);
	}

	u32 GraphicsContext::MinStorageBufferOffsetAlignment() const
	{
		return static_cast<u32>(internal->limits.minStorageBufferOffsetAlignment);
	}

	u32 GraphicsContext::MaxPushConstantsSize() const
	{
		return internal->limits.maxPushConstantsSize;
	}

	void GraphicsContext::WaitIdle()
	{
		internal->WaitIdle();
	}
}