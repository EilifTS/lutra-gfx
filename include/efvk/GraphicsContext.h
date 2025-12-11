#pragma once
#include <memory>

namespace efvk
{
	class Window;
	class GraphicsContextInternal;

	class GraphicsContext
	{
	public:
		GraphicsContext(const Window& window, const char* app_name);
		~GraphicsContext();

		GraphicsContext(const GraphicsContext&) = delete;
		GraphicsContext(GraphicsContext&&);
		GraphicsContext& operator=(const GraphicsContext&) = delete;
		GraphicsContext& operator=(GraphicsContext&&);

		void WaitIdle();

		std::unique_ptr<GraphicsContextInternal> internal{};
	};
}