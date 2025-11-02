#pragma once
#include <array>
#include "Utils.h"
#include "Event.h"

namespace ef
{
	class MouseState
	{
	public:
		MouseState() = default;

		const MousePosition& Position() const { return position; };
		const MousePosition& LastFramePosition() const { return last_frame_position; };
		bool HasMoved() const { return !((position.x == last_frame_position.x) && (position.y == last_frame_position.y)); }
		bool IsButtonDown(MouseButton button) const { return mouse_button_down[u32(button)]; };
		u32 ButtonPresses(MouseButton button) const { return mouse_button_press_count[u32(button)]; };
		u32 ButtonReleases(MouseButton button) const { return mouse_button_release_count[u32(button)]; };
		int Scroll() const { return scroll; };

	private:
		void startFrame();

		void addButtonPress(MouseButton button);
		void addButtonRelease(MouseButton button);
		void setPosition(const MousePosition& new_position) { position = new_position; };
		void setScroll(int new_scroll) { scroll += new_scroll; };

		static constexpr u32 MouseButtonCount = (u32)MouseButton::Left + 1;
		std::array<bool, MouseButtonCount> mouse_button_down{ false };
		std::array<u32, MouseButtonCount> mouse_button_press_count{ 0 };
		std::array<u32, MouseButtonCount> mouse_button_release_count{ 0 };

		MousePosition position{};
		MousePosition last_frame_position{};

		int scroll{ 0 };

		friend class InputManager;
	};

}