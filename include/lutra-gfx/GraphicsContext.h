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
		/* Same as above but also asks for optional features, see GraphicsFeatures. A feature the
		   GPU does not support is left disabled, it does not make the creation fail. */
		GraphicsContext(const char* app_name, const GraphicsFeatures& requested_features);
		GraphicsContext(const char* app_name, const Window& window, const GraphicsFeatures& requested_features);
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

		/* The optional features that were requested and are supported, and therefore enabled */
		const GraphicsFeatures& EnabledFeatures() const;

		std::unique_ptr<GraphicsContextInternal> internal{};
	};
}