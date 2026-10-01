#pragma once
#include <memory>
#include <lutra-gfx/CommonStructs.h>

namespace lgx
{
	class Window;
	class GraphicsContextInternal;

	class GraphicsContext
	{
	public:
		GraphicsContext(const char* app_name);
		GraphicsContext(const char* app_name, const Window& window);
		~GraphicsContext();

		GraphicsContext(const GraphicsContext&) = delete;
		GraphicsContext(GraphicsContext&&);
		GraphicsContext& operator=(const GraphicsContext&) = delete;
		GraphicsContext& operator=(GraphicsContext&&);

		void WaitIdle();

		/* Format of the swapchain images, to be used as GraphicsPipelineInfo::color_format for
		   pipelines rendering to the FrameManager's texture. Only valid for a context created
		   with a window. */
		ColorFormat SwapchainFormat() const;

		std::unique_ptr<GraphicsContextInternal> internal{};
	};
}