#pragma once
#include "Keyboard.h"
#include "Mouse.h"
#include "Event.h"
#include <span>

namespace lgx
{
	class InputManager
	{
	public:
		InputManager() {};

		void ResolveEvents(std::span<Event> events);

		const MouseState& Mouse() const { return mouse_state; };
		const KeyboardState& Keyboard() const { return keyboard_state; };

	private:
		void resolveEvent(const Event& event);

		static constexpr u32 EventTypeCount = (u32)EventType::MouseScroll + 1;

		MouseState mouse_state;
		KeyboardState keyboard_state;
		

		friend class Window;
	};

}