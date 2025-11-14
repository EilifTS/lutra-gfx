#pragma once
#include "vec.h"

namespace ef
{
	template <typename T>
	class RectangleT
	{
	public:
		RectangleT()
			: position(), size()
		{

		}
		RectangleT(const T& position, const T& size)
			: position(position), size(size)
		{

		}

		const T& Position() const { return position; };
		const T& Size() const { return size; };

		bool Contains(const T& point) const 
		{
			if (point.x < position.x) return false;
			if (point.y < position.y) return false;
			if (point.x >= position.x + size.x) return false;
			if (point.y >= position.y + size.y) return false;
			return true; 
		};
		bool Intersects(const RectangleT<T>& other) const
		{
			const bool is_left = position.x + size.x < other.position.x;
			const bool is_right = other.position.x + other.size.x < position.x;
			const bool is_over = position.y + size.y < other.position.y;
			const bool is_under = other.position.y + other.size.y < position.y;
			return !is_left && !is_right && !is_over && !is_under;
		}
		T Clamp(T point) const 
		{
			if (point.x < position.x)
			{
				point.x = position.x;
			}
			else if (point.x >= position.x + size.x)
			{
				point.x = position.x + size.x - 1;
			}
			if (point.y < position.y)
			{
				point.y = position.y;
			}
			else if (point.y >= position.y + size.y)
			{
				point.y = position.y + size.y - 1;
			}
			return point; 
		};


		RectangleT GetReduced(int amount) const
		{
			return RectangleT(T(position.x + amount, position.y + amount), T(size.x - amount * 2, size.y - amount * 2));
		};
		RectangleT GetReduced(const T& amount) const
		{
			return RectangleT(position + amount, size - amount * 2);
		};

	private:
		T position;
		T size;
	};


	//template<> class RectangleT<point2>;
	//template<> class RectangleT<vec2>;

	using Rectangle = RectangleT<point2>;
	using Rectanglef = RectangleT<vec2>;

	using Cuboid = RectangleT<point3>;
	using Cuboidf = RectangleT<vec3>;
}