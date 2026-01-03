#pragma once
#include <memory>

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

		std::unique_ptr<GraphicsContextInternal> internal{};
	};
}