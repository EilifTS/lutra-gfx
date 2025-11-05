#include <stdexcept>
#include <cassert>
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Window.h"

static int global_window_count = 0;

namespace
{
	inline efvk::KeyboardKey translateKey(int glfw_key)
	{
		switch (glfw_key)
		{
		case GLFW_KEY_A: return efvk::KeyboardKey::A;
		case GLFW_KEY_B: return efvk::KeyboardKey::B;
		case GLFW_KEY_C: return efvk::KeyboardKey::C;
		case GLFW_KEY_D: return efvk::KeyboardKey::D;
		case GLFW_KEY_E: return efvk::KeyboardKey::E;
		case GLFW_KEY_F: return efvk::KeyboardKey::F;
		case GLFW_KEY_G: return efvk::KeyboardKey::G;
		case GLFW_KEY_H: return efvk::KeyboardKey::H;
		case GLFW_KEY_I: return efvk::KeyboardKey::I;
		case GLFW_KEY_J: return efvk::KeyboardKey::J;
		case GLFW_KEY_K: return efvk::KeyboardKey::K;
		case GLFW_KEY_L: return efvk::KeyboardKey::L;
		case GLFW_KEY_M: return efvk::KeyboardKey::M;
		case GLFW_KEY_N: return efvk::KeyboardKey::N;
		case GLFW_KEY_O: return efvk::KeyboardKey::O;
		case GLFW_KEY_P: return efvk::KeyboardKey::P;
		case GLFW_KEY_Q: return efvk::KeyboardKey::Q;
		case GLFW_KEY_R: return efvk::KeyboardKey::R;
		case GLFW_KEY_S: return efvk::KeyboardKey::S;
		case GLFW_KEY_T: return efvk::KeyboardKey::T;
		case GLFW_KEY_U: return efvk::KeyboardKey::U;
		case GLFW_KEY_V: return efvk::KeyboardKey::V;
		case GLFW_KEY_W: return efvk::KeyboardKey::W;
		case GLFW_KEY_X: return efvk::KeyboardKey::X;
		case GLFW_KEY_Y: return efvk::KeyboardKey::Y;
		case GLFW_KEY_Z: return efvk::KeyboardKey::Z;
		case GLFW_KEY_LEFT: return efvk::KeyboardKey::LEFT;
		case GLFW_KEY_UP: return efvk::KeyboardKey::UP;
		case GLFW_KEY_RIGHT: return efvk::KeyboardKey::RIGHT;
		case GLFW_KEY_DOWN: return efvk::KeyboardKey::DOWN;
		case GLFW_KEY_SPACE: return efvk::KeyboardKey::SPACE;
		case GLFW_KEY_ESCAPE: return efvk::KeyboardKey::ESCAPE;
		case GLFW_KEY_LEFT_SHIFT: return efvk::KeyboardKey::SHIFT;
		case GLFW_KEY_LEFT_CONTROL: return efvk::KeyboardKey::CONTROL;
		default: return (efvk::KeyboardKey)-1;
		}
	}

	inline bool isValidKey(efvk::KeyboardKey key)
	{
		return (int)key != -1;
	}
}

namespace efvk
{
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
		payload.position.x = static_cast<int>(mouse_x);
		payload.position.y = static_cast<int>(mouse_y);
		events.push_back({ type, payload });
	}

	void GLFWMouseButtionCallback(GLFWwindow* glfw_window, int button, int action, int mods)
	{
		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
		assert(window);
		std::vector<Event>& events = window->events;

		if (action == GLFW_PRESS)
		{
			const EventType type = EventType::KeyboardPress;
			EventPayload payload{};
			if (button == GLFW_MOUSE_BUTTON_LEFT) payload.mouse_button = MouseButton::Left;
			else if (button == GLFW_MOUSE_BUTTON_MIDDLE) payload.mouse_button = MouseButton::Middle;
			else if (button == GLFW_MOUSE_BUTTON_RIGHT) payload.mouse_button = MouseButton::Right;

			events.push_back({ type, payload });
		}
		else if (action == GLFW_RELEASE)
		{
			const EventType type = EventType::KeyboardRelease;
			EventPayload payload{};
			if (button == GLFW_MOUSE_BUTTON_LEFT) payload.mouse_button = MouseButton::Left;
			else if (button == GLFW_MOUSE_BUTTON_MIDDLE) payload.mouse_button = MouseButton::Middle;
			else if (button == GLFW_MOUSE_BUTTON_RIGHT) payload.mouse_button = MouseButton::Right;

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
		: width(width), height(height)
	{
		if (global_window_count == 0)
		{
			if (!glfwInit())
			{
				std::cerr << "Failed to initialize GLFW" << std::endl;
			}
		}
		global_window_count++;

		if (!glfwVulkanSupported())
		{
			std::cerr << "GLFW with Vulkan is not supported on this system" << std::endl;
		}

		/* No OpenGL context */
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfw_window = glfwCreateWindow(width, height, window_name.c_str(), nullptr, nullptr);
		if (!glfw_window)
		{
			std::cerr << "Failed to create window" << std::endl;
		}

		//sf_window.setFramerateLimit(144);
		glfwSetWindowUserPointer(glfw_window, static_cast<void*>(this));
		is_open = true;
	}

	Window::~Window()
	{
		if (global_window_count == 1)
		{
			glfwTerminate();
		}
		global_window_count--;
		//sf_window.close();
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
	bool Window::IsOpen() const
	{
		return !glfwWindowShouldClose(glfw_window);
	}
	bool Window::IsFullScreen() const
	{
		return false;
	}
}
