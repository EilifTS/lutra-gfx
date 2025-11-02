#include <stdexcept>
#include <cassert>
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Window.h"

static int global_window_count = 0;

namespace
{
	inline ef::KeyboardKey translateKey(int glfw_key)
	{
		switch (glfw_key)
		{
		case GLFW_KEY_A: return ef::KeyboardKey::A;
		case GLFW_KEY_B: return ef::KeyboardKey::B;
		case GLFW_KEY_C: return ef::KeyboardKey::C;
		case GLFW_KEY_D: return ef::KeyboardKey::D;
		case GLFW_KEY_E: return ef::KeyboardKey::E;
		case GLFW_KEY_F: return ef::KeyboardKey::F;
		case GLFW_KEY_G: return ef::KeyboardKey::G;
		case GLFW_KEY_H: return ef::KeyboardKey::H;
		case GLFW_KEY_I: return ef::KeyboardKey::I;
		case GLFW_KEY_J: return ef::KeyboardKey::J;
		case GLFW_KEY_K: return ef::KeyboardKey::K;
		case GLFW_KEY_L: return ef::KeyboardKey::L;
		case GLFW_KEY_M: return ef::KeyboardKey::M;
		case GLFW_KEY_N: return ef::KeyboardKey::N;
		case GLFW_KEY_O: return ef::KeyboardKey::O;
		case GLFW_KEY_P: return ef::KeyboardKey::P;
		case GLFW_KEY_Q: return ef::KeyboardKey::Q;
		case GLFW_KEY_R: return ef::KeyboardKey::R;
		case GLFW_KEY_S: return ef::KeyboardKey::S;
		case GLFW_KEY_T: return ef::KeyboardKey::T;
		case GLFW_KEY_U: return ef::KeyboardKey::U;
		case GLFW_KEY_V: return ef::KeyboardKey::V;
		case GLFW_KEY_W: return ef::KeyboardKey::W;
		case GLFW_KEY_X: return ef::KeyboardKey::X;
		case GLFW_KEY_Y: return ef::KeyboardKey::Y;
		case GLFW_KEY_Z: return ef::KeyboardKey::Z;
		case GLFW_KEY_LEFT: return ef::KeyboardKey::LEFT;
		case GLFW_KEY_UP: return ef::KeyboardKey::UP;
		case GLFW_KEY_RIGHT: return ef::KeyboardKey::RIGHT;
		case GLFW_KEY_DOWN: return ef::KeyboardKey::DOWN;
		case GLFW_KEY_SPACE: return ef::KeyboardKey::SPACE;
		case GLFW_KEY_ESCAPE: return ef::KeyboardKey::ESCAPE;
		case GLFW_KEY_LEFT_SHIFT: return ef::KeyboardKey::SHIFT;
		case GLFW_KEY_LEFT_CONTROL: return ef::KeyboardKey::CONTROL;
		default: return (ef::KeyboardKey)-1;
		}
	}

	inline bool isValidKey(ef::KeyboardKey key)
	{
		return (int)key != -1;
	}
}

namespace ef
{
	void GLFWKeyCallback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods)
	{
		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
		assert(window);
		std::vector<ef::Event>& events = window->events;

		if (action == GLFW_PRESS)
		{
			const ef::EventType type = ef::EventType::KeyboardPress;
			ef::EventPayload payload{};
			payload.key = translateKey(key);

			const bool is_valid_event = isValidKey(payload.key);
			if (is_valid_event)
			{
				events.push_back({ type, payload });
			}
		}
		else if (action == GLFW_RELEASE)
		{
			const ef::EventType type = ef::EventType::KeyboardRelease;
			ef::EventPayload payload{};
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
		std::vector<ef::Event>& events = window->events;

		const ef::EventType type = ef::EventType::MouseMove;
		ef::EventPayload payload{};
		payload.position.x = static_cast<int>(mouse_x);
		payload.position.y = static_cast<int>(mouse_y);
		events.push_back({ type, payload });
	}

	void GLFWMouseButtionCallback(GLFWwindow* glfw_window, int button, int action, int mods)
	{
		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
		assert(window);
		std::vector<ef::Event>& events = window->events;

		if (action == GLFW_PRESS)
		{
			const ef::EventType type = ef::EventType::KeyboardPress;
			ef::EventPayload payload{};
			if (button == GLFW_MOUSE_BUTTON_LEFT) payload.mouse_button = ef::MouseButton::Left;
			else if (button == GLFW_MOUSE_BUTTON_MIDDLE) payload.mouse_button = ef::MouseButton::Middle;
			else if (button == GLFW_MOUSE_BUTTON_RIGHT) payload.mouse_button = ef::MouseButton::Right;

			events.push_back({ type, payload });
		}
		else if (action == GLFW_RELEASE)
		{
			const ef::EventType type = ef::EventType::KeyboardRelease;
			ef::EventPayload payload{};
			if (button == GLFW_MOUSE_BUTTON_LEFT) payload.mouse_button = ef::MouseButton::Left;
			else if (button == GLFW_MOUSE_BUTTON_MIDDLE) payload.mouse_button = ef::MouseButton::Middle;
			else if (button == GLFW_MOUSE_BUTTON_RIGHT) payload.mouse_button = ef::MouseButton::Right;

			events.push_back({ type, payload });
		}
	}

	void GLFWMouseScrollCallback(GLFWwindow* glfw_window, double x_delta, double y_delta)
	{
		(void)x_delta;

		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
		assert(window);
		std::vector<ef::Event>& events = window->events;

		const ef::EventType type = ef::EventType::MouseScroll;
		ef::EventPayload payload{};
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
