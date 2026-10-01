#include <stdexcept>
#include <cassert>
#include <cstdlib>
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <lutra-gfx/Window.h>

static int global_window_count = 0;

namespace
{
	inline lgx::KeyboardKey translateKey(int glfw_key)
	{
		switch (glfw_key)
		{
		case GLFW_KEY_A: return lgx::KeyboardKey::A;
		case GLFW_KEY_B: return lgx::KeyboardKey::B;
		case GLFW_KEY_C: return lgx::KeyboardKey::C;
		case GLFW_KEY_D: return lgx::KeyboardKey::D;
		case GLFW_KEY_E: return lgx::KeyboardKey::E;
		case GLFW_KEY_F: return lgx::KeyboardKey::F;
		case GLFW_KEY_G: return lgx::KeyboardKey::G;
		case GLFW_KEY_H: return lgx::KeyboardKey::H;
		case GLFW_KEY_I: return lgx::KeyboardKey::I;
		case GLFW_KEY_J: return lgx::KeyboardKey::J;
		case GLFW_KEY_K: return lgx::KeyboardKey::K;
		case GLFW_KEY_L: return lgx::KeyboardKey::L;
		case GLFW_KEY_M: return lgx::KeyboardKey::M;
		case GLFW_KEY_N: return lgx::KeyboardKey::N;
		case GLFW_KEY_O: return lgx::KeyboardKey::O;
		case GLFW_KEY_P: return lgx::KeyboardKey::P;
		case GLFW_KEY_Q: return lgx::KeyboardKey::Q;
		case GLFW_KEY_R: return lgx::KeyboardKey::R;
		case GLFW_KEY_S: return lgx::KeyboardKey::S;
		case GLFW_KEY_T: return lgx::KeyboardKey::T;
		case GLFW_KEY_U: return lgx::KeyboardKey::U;
		case GLFW_KEY_V: return lgx::KeyboardKey::V;
		case GLFW_KEY_W: return lgx::KeyboardKey::W;
		case GLFW_KEY_X: return lgx::KeyboardKey::X;
		case GLFW_KEY_Y: return lgx::KeyboardKey::Y;
		case GLFW_KEY_Z: return lgx::KeyboardKey::Z;
		case GLFW_KEY_LEFT: return lgx::KeyboardKey::LEFT;
		case GLFW_KEY_UP: return lgx::KeyboardKey::UP;
		case GLFW_KEY_RIGHT: return lgx::KeyboardKey::RIGHT;
		case GLFW_KEY_DOWN: return lgx::KeyboardKey::DOWN;
		case GLFW_KEY_SPACE: return lgx::KeyboardKey::SPACE;
		case GLFW_KEY_ESCAPE: return lgx::KeyboardKey::ESCAPE;
		case GLFW_KEY_LEFT_SHIFT: return lgx::KeyboardKey::SHIFT;
		case GLFW_KEY_LEFT_CONTROL: return lgx::KeyboardKey::CONTROL;
		default: return (lgx::KeyboardKey)-1;
		}
	}

	inline bool isValidKey(lgx::KeyboardKey key)
	{
		return (int)key != -1;
	}

	inline lgx::MouseButton translateMouseButton(int glfw_button)
	{
		switch (glfw_button)
		{
		case GLFW_MOUSE_BUTTON_LEFT: return lgx::MouseButton::Left;
		case GLFW_MOUSE_BUTTON_MIDDLE: return lgx::MouseButton::Middle;
		case GLFW_MOUSE_BUTTON_RIGHT: return lgx::MouseButton::Right;
		default: return (lgx::MouseButton)-1;
		}
	}

	inline bool isValidMouseButton(lgx::MouseButton button)
	{
		return (int)button != -1;
	}
}

namespace lgx
{
	/* Ratio between framebuffer pixels and window screen coordinates (2 on Retina, 1 elsewhere) */
	static void getPixelScale(GLFWwindow* glfw_window, double& scale_x, double& scale_y)
	{
		int window_w = 0, window_h = 0, fb_w = 0, fb_h = 0;
		glfwGetWindowSize(glfw_window, &window_w, &window_h);
		glfwGetFramebufferSize(glfw_window, &fb_w, &fb_h);
		scale_x = window_w > 0 ? static_cast<double>(fb_w) / window_w : 1.0;
		scale_y = window_h > 0 ? static_cast<double>(fb_h) / window_h : 1.0;
	}

	void GLFWKeyCallback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods)
	{
		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
		assert(window);
		std::vector<Event>& events = window->events;

		if (action == GLFW_PRESS)
		{
			const EventType type = EventType::KeyboardPress;
			EventPayload payload{};
			payload.key = translateKey(key);

			const bool is_valid_event = isValidKey(payload.key);
			if (is_valid_event)
			{
				events.push_back({ type, payload });
			}
		}
		else if (action == GLFW_RELEASE)
		{
			const EventType type = EventType::KeyboardRelease;
			EventPayload payload{};
			payload.key = translateKey(key);

			const bool is_valid_event = isValidKey(payload.key);
			if (is_valid_event)
			{
				events.push_back({ type, payload });
			}
		}
	}

	void GLFWMouseMoveCallback(GLFWwindow* glfw_window, double mouse_x, double mouse_y)
	{
		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
		assert(window);
		std::vector<Event>& events = window->events;

		const EventType type = EventType::MouseMove;
		EventPayload payload{};

		/* GLFW reports the cursor in screen coordinates, convert to framebuffer pixels */
		double scale_x = 1.0, scale_y = 1.0;
		getPixelScale(glfw_window, scale_x, scale_y);
		payload.position.x = static_cast<int>(mouse_x * scale_x);
		payload.position.y = static_cast<int>(mouse_y * scale_y);
		events.push_back({ type, payload });
	}

	void GLFWMouseButtonCallback(GLFWwindow* glfw_window, int button, int action, int mods)
	{
		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
		assert(window);
		std::vector<Event>& events = window->events;

		const MouseButton mouse_button = translateMouseButton(button);
		if (!isValidMouseButton(mouse_button))
		{
			return;
		}

		if (action == GLFW_PRESS)
		{
			const EventType type = EventType::MouseButtonPress;
			EventPayload payload{};
			payload.mouse_button = mouse_button;

			events.push_back({ type, payload });
		}
		else if (action == GLFW_RELEASE)
		{
			const EventType type = EventType::MouseButtonRelease;
			EventPayload payload{};
			payload.mouse_button = mouse_button;

			events.push_back({ type, payload });
		}
	}

	void GLFWMouseScrollCallback(GLFWwindow* glfw_window, double x_delta, double y_delta)
	{
		(void)x_delta;

		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
		assert(window);
		std::vector<Event>& events = window->events;

		const EventType type = EventType::MouseScroll;
		EventPayload payload{};
		payload.mouse_scroll_amount = static_cast<int>(y_delta);
		events.push_back({ type, payload });
	}

	Window::Window(unsigned int width, unsigned int height, const std::string& window_name)
	{
		if (global_window_count == 0)
		{
			const bool glfw_init_ok = glfwInit();
			if (!glfw_init_ok)
			{
				std::cerr << "Failed to initialize GLFW" << std::endl;
			}
			assert(glfw_init_ok);
		}
		global_window_count++;

		const bool vulkan_supported = glfwVulkanSupported();
		if (!vulkan_supported)
		{
			std::cerr << "GLFW with Vulkan is not supported on this system" << std::endl;
			std::abort();
		}

		/* No OpenGL context */
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfw_window = glfwCreateWindow(width, height, window_name.c_str(), nullptr, nullptr);
		if (!glfw_window)
		{
			std::cerr << "Failed to create window" << std::endl;
		}
		assert(glfw_window);

		glfwSetKeyCallback(glfw_window, GLFWKeyCallback);
		glfwSetCursorPosCallback(glfw_window, GLFWMouseMoveCallback);
		glfwSetMouseButtonCallback(glfw_window, GLFWMouseButtonCallback);
		glfwSetScrollCallback(glfw_window, GLFWMouseScrollCallback);

		//sf_window.setFramerateLimit(144);
		glfwSetWindowUserPointer(glfw_window, static_cast<void*>(this));
		is_open = true;
	}

	Window::Window(Window&& rhs) noexcept
		: glfw_window(rhs.glfw_window), is_open(rhs.is_open), events(std::move(rhs.events))
	{
		rhs.glfw_window = nullptr;

		/* Re-point the user pointer set up in the constructor at the moved-to object,
		   otherwise the GLFW callbacks above would dereference a dangling Window*. */
		if (glfw_window != nullptr)
		{
			glfwSetWindowUserPointer(glfw_window, static_cast<void*>(this));
		}
	}

	Window& Window::operator=(Window&& rhs) noexcept
	{
		if (this == &rhs)
		{
			return *this;
		}

		/* Destroy whatever this window currently owns before taking over rhs's */
		if (glfw_window != nullptr)
		{
			glfwDestroyWindow(glfw_window);

			if (global_window_count == 1)
			{
				glfwTerminate();
			}
			global_window_count--;
		}

		glfw_window = rhs.glfw_window;
		is_open = rhs.is_open;
		events = std::move(rhs.events);

		rhs.glfw_window = nullptr;

		if (glfw_window != nullptr)
		{
			glfwSetWindowUserPointer(glfw_window, static_cast<void*>(this));
		}

		return *this;
	}

	Window::~Window()
	{
		/* A moved-from Window owns nothing: skip destruction and, importantly, don't
		   touch global_window_count a second time for the same underlying window. */
		if (glfw_window == nullptr)
		{
			return;
		}

		glfwDestroyWindow(glfw_window);

		if (global_window_count == 1)
		{
			glfwTerminate();
		}
		global_window_count--;
	}

	std::vector<Event> Window::RetrieveEvents()
	{
		glfwPollEvents();

		return std::move(events);
	}

	WindowHandle Window::GetHandle() const
	{
		return (WindowHandle)glfw_window;
	}
	unsigned int Window::Width() const
	{
		int fb_w = 0, fb_h = 0;
		if (glfw_window != nullptr)
		{
			glfwGetFramebufferSize(glfw_window, &fb_w, &fb_h);
		}
		return static_cast<unsigned int>(fb_w);
	}

	unsigned int Window::Height() const
	{
		int fb_w = 0, fb_h = 0;
		if (glfw_window != nullptr)
		{
			glfwGetFramebufferSize(glfw_window, &fb_w, &fb_h);
		}
		return static_cast<unsigned int>(fb_h);
	}

	bool Window::IsOpen() const
	{
		return !glfwWindowShouldClose(glfw_window);
	}
	bool Window::IsFullScreen() const
	{
		return false;
	}
}
