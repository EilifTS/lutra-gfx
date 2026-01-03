#pragma once
#include <array>
#include "CommonStructs.h"
#include "Event.h"

namespace lgx
{
	class KeyboardState
	{
	public:
		KeyboardState() = default;

		bool IsKeyDown(KeyboardKey key) const { return keyboard_key_down[u32(key)]; };
		u32 KeyPresses(KeyboardKey key) const { return keyboard_key_press_count[u32(key)]; };
		u32 KeyReleases(KeyboardKey key) const { return keyboard_key_release_count[u32(key)]; };

	private:
		void startFrame();

		void addKeyPress(KeyboardKey key);
		void addKeyRelease(KeyboardKey key);

		static constexpr u32 KeyboardKeyCount = (u32)lgx::KeyboardKey::ESCAPE + 1;

		std::array<bool, KeyboardKeyCount> keyboard_key_down { false };
		std::array<u32, KeyboardKeyCount> keyboard_key_press_count { 0 };
		std::array<u32, KeyboardKeyCount> keyboard_key_release_count { 0 };

		friend class InputManager;
	};

}
