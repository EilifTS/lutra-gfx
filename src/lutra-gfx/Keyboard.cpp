#include <lutra-gfx/Keyboard.h>

namespace lgx
{
	void KeyboardState::startFrame()
	{
		std::fill(keyboard_key_press_count.begin(), keyboard_key_press_count.end(), 0);
		std::fill(keyboard_key_release_count.begin(), keyboard_key_release_count.end(), 0);
	}

	void KeyboardState::addKeyPress(KeyboardKey key)
	{
		keyboard_key_down[u32(key)] = true;
		keyboard_key_press_count[u32(key)]++;
	}

	void KeyboardState::addKeyRelease(KeyboardKey key)
	{
		keyboard_key_down[u32(key)] = false;
		keyboard_key_release_count[u32(key)]++;
	}
}