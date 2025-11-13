#pragma once
#include "Utils.h"

namespace efvk
{
	enum class EventType
	{
		WindowClose,
		KeyboardPress,
		KeyboardRelease,
		MouseButtonPress,
		MouseButtonRelease,
		MouseMove,
		MouseScroll,
	};

	enum class MouseButton
	{
		Right, Middle, Left
	};

	enum class KeyboardKey
	{
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		LEFT, UP, RIGHT, DOWN,
		SPACE, SHIFT, CONTROL, ESCAPE
	};

	struct EventPayload
	{
		union
		{
			KeyboardKey key;
			MouseButton mouse_button;
			int mouse_scroll_amount;
			MousePosition position;
		};
	};

	struct Event
	{
		EventType type;
		EventPayload payload;
	};
}