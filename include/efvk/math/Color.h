#pragma once
#include "vec.h"

namespace ef
{

	class Color
	{
	public:
		u8 r{ 255 };
		u8 g{ 255 };
		u8 b{ 255 };
		u8 a{ 255 };
		inline Color() {};
		inline Color(u8 r, u8 g, u8 b)
			: r(r), g(g), b(b)
		{

		}
		inline Color(u8 r, u8 g, u8 b, u8 a)
			: r(r), g(g), b(b), a(a)
		{

		}
		inline Color(const vec4& c)
		{
			assert(c.x >= 0.0f && c.x <= 1.0f);
			assert(c.y >= 0.0f && c.y <= 1.0f);
			assert(c.z >= 0.0f && c.z <= 1.0f);
			assert(c.w >= 0.0f && c.w <= 1.0f);
			auto temp = c * 255.9999f;
			r = u8(temp.x);
			g = u8(temp.y);
			b = u8(temp.z);
			a = u8(temp.w);
		}
		inline Color(const vec3& c)
			: Color(vec4(c, 1.0f))
		{
		}

		vec3 ToVec3() const
		{
			return { float(r) / 255.0f, float(g) / 255.0f, float(b) / 255.0f};
		}
		vec4 ToVec4() const
		{
			return { float(r) / 255.0f, float(g) / 255.0f, float(b) / 255.0f, float(a) / 255.0f };
		}

		static Color Green()
		{
			return { 0, 255, 0, 255 };
		}
		static Color Blue()
		{
			return { 0, 0, 255, 255 };
		}
		static Color Red()
		{
			return { 255, 0, 0, 255 };
		}
	};
}