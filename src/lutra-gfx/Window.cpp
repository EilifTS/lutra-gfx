#include <stdexcept>
#include <cassert>
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
}

namespace lgx
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

	void GLFWMouseButtonCallback(GLFWwindow* glfw_window, int button, int action, int mods)
	{
		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
		assert(window);
		std::vector<Event>& events = window->events;

		if (action == GLFW_PRESS)
		{
			const EventType type = EventType::MouseButtonPress;
			EventPayload payload{};
			if (button == GLFW_MOUSE_BUTTON_LEFT) payload.mouse_button = MouseButton::Left;
			else if (button == GLFW_MOUSE_BUTTON_MIDDLE) payload.mouse_button = MouseButton::Middle;
			else if (button == GLFW_MOUSE_BUTTON_RIGHT) payload.mouse_button = MouseButton::Right;

			events.push_back({ type, payload });
		}
		else if (action == GLFW_RELEASE)
		{
			const EventType type = EventType::MouseButtonRelease;
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

		glfwSetKeyCallback(glfw_window, GLFWKeyCallback);
		glfwSetCursorPosCallback(glfw_window, GLFWMouseMoveCallback);
		glfwSetMouseButtonCallback(glfw_window, GLFWMouseButtionCallback);
		glfwSetScrollCallback(glfw_window, GLFWMouseScrollCallback);

		//sf_window.setFramerateLimit(144);
		glfwSetWindowUserPointer(glfw_window, static_cast<void*>(this));
		is_open = true;
	}

	Window::~Window()
	{
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
	bool Window::IsOpen() const
	{
		return !glfwWindowShouldClose(glfw_window);
	}
	bool Window::IsFullScreen() const
	{
		return false;
	}
}
