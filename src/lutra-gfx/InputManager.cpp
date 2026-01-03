#include <lutra-gfx/InputManager.h>

namespace lgx
{
	void InputManager::ResolveEvents(std::span<Event> events)
	{
		mouse_state.startFrame();
		keyboard_state.startFrame();

		for (u32 i = 0; i < u32(events.size()); i++)
		{
			resolveEvent(events[i]);
		}
	}

	void InputManager::resolveEvent(const Event& event)
	{
		switch (event.type)
		{
		case EventType::KeyboardPress:
			keyboard_state.addKeyPress(event.payload.key);
			break;
		case EventType::KeyboardRelease:
			keyboard_state.addKeyRelease(event.payload.key);
			break;
		case EventType::MouseButtonPress:
			mouse_state.addButtonPress(event.payload.mouse_button);
			break;
		case EventType::MouseButtonRelease:
			mouse_state.addButtonRelease(event.payload.mouse_button);
			break;
		case EventType::MouseMove:
			mouse_state.setPosition(event.payload.position);
			break;
		case EventType::MouseScroll:
			mouse_state.setScroll(event.payload.mouse_scroll_amount);
			break;
		case EventType::WindowClose:
			/* Do nothing */
			break;
		}
	}
}