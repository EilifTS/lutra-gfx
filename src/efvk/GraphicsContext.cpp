#include <efvk/GraphicsContext.h>

#include "internal/GraphicsContextInternal.h"

namespace efvk
{
	GraphicsContext::GraphicsContext(const Window& window, const char* app_name)
	{
		internal = std::make_unique<GraphicsContextInternal>(window, app_name);
	}

	GraphicsContext::~GraphicsContext() {}

	GraphicsContext::GraphicsContext(GraphicsContext&&) = default;
	GraphicsContext& GraphicsContext::operator=(GraphicsContext&&) = default;

	void GraphicsContext::WaitIdle()
	{
		internal->WaitIdle();
	}
}