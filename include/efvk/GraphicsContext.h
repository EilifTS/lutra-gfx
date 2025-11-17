#pragma once
#include <memory>

#include "Window.h"
#include "Utils.h"

namespace efvk
{
	class FrameManager;

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

		struct Impl;
		std::unique_ptr<Impl> pimpl{};
	};
}