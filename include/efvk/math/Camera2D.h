#pragma once
#include "Math.h"

namespace ef
{
	class Camera2D
	{
	public:
		Camera2D()
			: position(), window_size(), zoom(1.0f)
		{

		}

		Camera2D(const vec2& window_size)
			: position(), window_size(window_size), zoom(1.0f)
		{

		}

		inline vec2& GetPosition() { return position; };
		inline const vec2& GetPosition() const { return position; };

		inline vec2 GetCenter() const { return position + window_size * 0.5f; };

		inline float& GetZoom() { return zoom; };

		inline vec2 WorldToScreenPosition(const vec2& pos) const
		{
			return (pos - position) / zoom + window_size * 0.5f;
		}
		inline vec2 ScreenToWorldPosition(const vec2& pos) const
		{
			return (pos - window_size * 0.5f) * zoom + position;
		}
		inline vec2 WorldToScreenSize(const vec2& size) const
		{
			return size / zoom;
		}
		inline vec2 ScreenToWorldSize(const vec2& size) const
		{
			return size * zoom;
		}
		inline const vec2& GetCameraSize() const { return window_size; };

	private:
		vec2 position;
		vec2 window_size;
		float zoom;
	};
}