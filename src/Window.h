#pragma once
#include <functional>
#include <vector>
#include <string>

#include "Event.h"

class GLFWwindow;
namespace ef
{
	class Window;
	using WindowHandle = void*;

	//using MessageHook = std::function<bool(Window&, const sf::Event&)>;

	void GLFWKeyCallback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods);
	void GLFWMouseMoveCallback(GLFWwindow* glfw_window, double mouse_x, double mouse_y);
	void GLFWMouseButtionCallback(GLFWwindow* glfw_window, int button, int action, int mods);
	void GLFWMouseScrollCallback(GLFWwindow* glfw_window, double x_delta, double y_delta);

	class Window
	{
	public:
		Window(unsigned int window_width, unsigned int window_height, const std::string& window_name);
		~Window();

		std::vector<Event> RetrieveEvents();

		WindowHandle GetHandle() const;
		bool IsOpen() const;
		bool IsFullScreen() const;
		unsigned int Width() const { return width; };
		unsigned int Height() const { return height; };
		//void AddMessageHook(const MessageHook& message_hook) { message_hooks.push_back(message_hook); };

		//void Clear() { sf_window.clear(); }
		//void Display() { sf_window.display(); }

		//sf::RenderWindow& GetSFWindow() { return sf_window; }

	private:
		GLFWwindow* glfw_window{};

		unsigned int width{};
		unsigned int height{};
		bool is_open{ true };

		std::vector<Event> events{};

		//std::vector<MessageHook> message_hooks{};

		friend void GLFWKeyCallback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods);
		friend void GLFWMouseMoveCallback(GLFWwindow* glfw_window, double mouse_x, double mouse_y);
		friend void GLFWMouseButtionCallback(GLFWwindow* glfw_window, int button, int action, int mods);
		friend void GLFWMouseScrollCallback(GLFWwindow* glfw_window, double x_delta, double y_delta);
	};
}