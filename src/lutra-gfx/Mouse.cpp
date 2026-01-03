#include <efvk/Mouse.h>

namespace efvk
{
	void MouseState::startFrame()
	{
		std::fill(mouse_button_press_count.begin(), mouse_button_press_count.end(), 0);
		std::fill(mouse_button_release_count.begin(), mouse_button_release_count.end(), 0);
		last_frame_position = position;
		scroll = 0;
	}

	void MouseState::addButtonPress(MouseButton button)
	{
		mouse_button_down[u32(button)] = true;
		mouse_button_press_count[u32(button)]++;
	}

	void MouseState::addButtonRelease(MouseButton button)
	{
		mouse_button_down[u32(button)] = false;
		mouse_button_release_count[u32(button)]++;
	}
}
