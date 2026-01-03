#include <lutra-gfx/GraphicsContext.h>

#include "internal/GraphicsContextInternal.h"

namespace lgx
{
	GraphicsContext::GraphicsContext(const char* app_name)
	{
		internal = std::make_unique<GraphicsContextInternal>(app_name);
	}

	GraphicsContext::GraphicsContext(const char* app_name, const Window& window)
	{
		internal = std::make_unique<GraphicsContextInternal>(app_name, window);
	}

	GraphicsContext::~GraphicsContext() {}

	GraphicsContext::GraphicsContext(GraphicsContext&&) = default;
	GraphicsContext& GraphicsContext::operator=(GraphicsContext&&) = default;

	void GraphicsContext::WaitIdle()
	{
		internal->WaitIdle();
	}
}