#pragma once
#include <EFLib/Utility/Assert.h>
#include <cmath>
#include <EFLib/Common/IntDefs.h>

namespace ef
{
	template<typename T>
	class tpoint2
	{
	public:
		T x;
		T y;

	public:
		inline tpoint2()
			: x(0), y(0)
		{}
		explicit inline tpoint2(T v)
			: x(v), y(v)
		{}
		inline tpoint2(T x, T y)
			: x(x), y(y)
		{}

		inline tpoint2& operator+=(const tpoint2& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			return *this;
		}
		inline tpoint2& operator-=(const tpoint2& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			return *this;
		}
		inline tpoint2 operator+(const tpoint2& rhs) const
		{
			return tpoint2(x + rhs.x, y + rhs.y);
		}
		inline tpoint2 operator-(const tpoint2& rhs) const
		{
			return tpoint2(x - rhs.x, y - rhs.y);
		}
		inline tpoint2 operator*(int rhs) const
		{
			return tpoint2(x * rhs, y * rhs);
		}
		inline tpoint2 operator*(const tpoint2& rhs) const
		{
			return tpoint2(x * rhs.x, y * rhs.y);
		}
		inline tpoint2 operator/(int rhs) const
		{
			return tpoint2(x / rhs, y / rhs);
		}
		inline tpoint2 operator/(const tpoint2& rhs) const
		{
			return tpoint2(x / rhs.x, y / rhs.y);
		}
		inline tpoint2 operator>>(int rhs) const
		{
			return tpoint2(x >> rhs, y >> rhs);
		}
		inline tpoint2 operator<<(int rhs) const
		{
			return tpoint2(x << rhs, y << rhs);
		}
		inline tpoint2 operator-() const
		{
			tpoint2 temp;
			temp -= *this;
			return temp;
		};
		inline tpoint2& operator=(const tpoint2& rhs)
		{
			this->x = rhs.x;
			this->y = rhs.y;
			return *this;
		}

		inline bool operator==(const tpoint2& rhs) const
		{
			return x == rhs.x && y == rhs.y;
		}
	};

	template <typename T>
	inline tpoint2<T> operator*(T lhs, const tpoint2<T>& a)
	{
		return a * lhs;
	};

	using point2 = tpoint2<int>;
	using point2u = tpoint2<u32>;

	template<typename T>
	class tpoint3
	{
	public:
		T x;
		T y;
		T z;

	public:
		inline tpoint3()
			: x(0), y(0), z(0)
		{}
		explicit inline tpoint3(T v)
			: x(v), y(v), z(v)
		{}
		inline tpoint3(T x, T y, T z)
			: x(x), y(y), z(z)
		{}

		inline tpoint3& operator+=(const tpoint3& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			return *this;
		}
		inline tpoint3& operator-=(const tpoint3& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			return *this;
		}
		inline tpoint3 operator+(const tpoint3& rhs) const
		{
			return tpoint3(x + rhs.x, y + rhs.y, z + rhs.z);
		}
		inline tpoint3 operator-(const tpoint3& rhs) const
		{
			return tpoint3(x - rhs.x, y - rhs.y, z - rhs.z);
		}
		inline tpoint3 operator*(int rhs) const
		{
			return tpoint3(x * rhs, y * rhs, z * rhs);
		}
		inline tpoint3 operator*(const tpoint3& rhs) const
		{
			return tpoint3(x * rhs.x, y * rhs.y, z * rhs.z);
		}
		inline tpoint3 operator/(int rhs) const
		{
			return tpoint3(x / rhs, y / rhs, z / rhs);
		}
		inline tpoint3 operator/(const tpoint3& rhs) const
		{
			return tpoint3(x / rhs.x, y / rhs.y, z / rhs.z);
		}
		inline tpoint3 operator>>(int rhs) const
		{
			return tpoint3(x >> rhs, y >> rhs, z >> rhs);
		}
		inline tpoint3 operator<<(int rhs) const
		{
			return tpoint3(x << rhs, y << rhs, z << rhs);
		}
		inline tpoint3 operator-() const
		{
			tpoint3 temp;
			temp -= *this;
			return temp;
		};

		inline bool operator==(const tpoint3& rhs) const
		{
			return x == rhs.x && y == rhs.y && z == rhs.z;
		}
	};
	
	template <typename T>
	inline tpoint3<T> operator*(T lhs, const tpoint3<T>& a)
	{
		return a * lhs;
	};

	using point3 = tpoint3<int>;
	using point3u = tpoint3<u32>;

	template<typename T>
	class tpoint4
	{
	public:
		T x;
		T y;
		T z;
		T w;

	public:
		inline tpoint4()
			: x(0), y(0), z(0), w(0)
		{}
		explicit inline tpoint4(T v)
			: x(v), y(v), z(v), w(v)
		{}
		inline tpoint4(T x, T y, T z, T w)
			: x(x), y(y), z(z), w(w)
		{}

		inline tpoint4& operator+=(const tpoint4& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			w += rhs.w;
			return *this;
		}
		inline tpoint4& operator-=(const tpoint4& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			w -= rhs.w;
			return *this;
		}
		inline tpoint4 operator+(const tpoint4& rhs) const
		{
			return tpoint4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
		}
		inline tpoint4 operator-(const tpoint4& rhs) const
		{
			return tpoint4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
		}
		inline tpoint4 operator*(int rhs) const
		{
			return tpoint4(x * rhs, y * rhs, z * rhs, w * rhs);
		}
		inline tpoint4 operator*(const tpoint4& rhs) const
		{
			return tpoint4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
		}
		inline tpoint4 operator/(int rhs) const
		{
			return tpoint4(x / rhs, y / rhs, z / rhs, w / rhs);
		}
		inline tpoint4 operator/(const tpoint4& rhs) const
		{
			return tpoint4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
		}
		inline tpoint4 operator>>(int rhs) const
		{
			return tpoint4(x >> rhs, y >> rhs, z >> rhs, w >> rhs);
		}
		inline tpoint4 operator<<(int rhs) const
		{
			return tpoint4(x << rhs, y << rhs, z << rhs, w << rhs);
		}
		inline tpoint4 operator-() const
		{
			tpoint4 temp;
			temp -= *this;
			return temp;
		};

		inline bool operator==(const tpoint4& rhs) const
		{
			return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
		}
	};

	template <typename T>
	inline tpoint4<T> operator*(T lhs, const tpoint4<T>& a)
	{
		return a * lhs;
	};

	using point4 = tpoint4<int>;
	using point4u = tpoint4<u32>;

	class vec2
	{
	public:
		float x, y;

	public:
		inline vec2()
			: x(0.0f), y(0.0f) {};
		explicit inline vec2(float v)
			: x(v), y(v) {};
		inline vec2(float x, float y)
			: x(x), y(y) {};
		inline explicit vec2(const point2& rhs)
			: x((float)rhs.x), y((float)rhs.y) {};

		inline float Dot(const vec2& rhs) const
		{
			return x * rhs.x + y * rhs.y;
		};
		inline float Cross(const vec2& rhs) const
		{
			return x * rhs.y - y * rhs.x;
		}

		inline float LengthSquared() const
		{
			return Dot(*this);
		};
		inline float Length() const
		{
			return std::sqrt(LengthSquared());
		};
		inline float Angle(const vec2& rhs) const
		{
			return std::acos(Dot(rhs) / (Length() * rhs.Length()));
		}
		inline float Angle() const
		{
			return atan2f(y, x);
		}

		inline bool IsZero() const
		{
			return x == 0.0f && y == 0.0f;
		}
		inline void Normalize()
		{
			*this /= Length();
		}
		inline vec2 GetNormalized() const
		{
			return *this / Length();
		}
		inline void Rotate(float angle)
		{
			float c = cosf(angle);
			float s = sinf(angle);
			float ox = x;
			float oy = y;
			x = c * ox - s * oy;
			y = s * ox + c * oy;
		}

		inline vec2& operator+=(const vec2& rhs)
		{
			this->x += rhs.x;
			this->y += rhs.y;
			return *this;
		};
		inline vec2& operator-=(const vec2& rhs)
		{
			this->x -= rhs.x;
			this->y -= rhs.y;
			return *this;
		};
		inline vec2& operator*=(const vec2& rhs)
		{
			this->x *= rhs.x;
			this->y *= rhs.y;
			return *this;
		}
		inline vec2& operator/=(const vec2& rhs)
		{
			this->x /= rhs.x;
			this->y /= rhs.y;
			return *this;
		}
		inline vec2 operator+(const vec2& rhs) const
		{
			vec2 temp(*this);
			temp += rhs;
			return temp;
		};
		inline vec2 operator-(const vec2& rhs) const
		{
			vec2 temp(*this);
			temp -= rhs;
			return temp;
		};
		inline vec2 operator*(const vec2& rhs) const
		{
			vec2 temp(*this);
			temp *= rhs;
			return temp;
		}
		inline vec2 operator/(const vec2& rhs) const
		{
			vec2 temp(*this);
			temp /= rhs;
			return temp;
		}
		inline vec2 operator-() const
		{
			vec2 temp;
			temp -= *this;
			return temp;
		};
		inline vec2& operator*=(float rhs)
		{
			this->x *= rhs;
			this->y *= rhs;
			return *this;
		};
		inline vec2& operator/=(float rhs)
		{
			this->x /= rhs;
			this->y /= rhs;
			return *this;
		};
		inline vec2 operator*(float rhs) const
		{
			vec2 temp(*this);
			temp *= rhs;
			return temp;
		};
		inline vec2 operator/(float rhs) const
		{
			vec2 temp(*this);
			temp /= rhs;
			return temp;
		};
		inline bool operator==(const vec2& rhs) const
		{
			return this->x == rhs.x && this->y == rhs.y;
		};
		inline float operator[](int i) const
		{
			Assert(i >= 0 && i < 2);
			if (i == 0) return x;
			if (i == 1) return y;
			return 0.0f;
		};
		inline float& operator[](int i)
		{
			Assert(i >= 0 && i < 2);
			if (i == 0) return x;
			if (i == 1) return y;
			return x;
		};

		explicit inline operator point2() const
		{
			return point2((int)x, (int)y);
		}
	};

	inline vec2 operator*(float lhs, const vec2& a)
	{
		return a * lhs;
	};

	class vec3
	{
	public:
		float x, y, z;

	public:
		inline vec3()
			: x(0.0f), y(0.0f), z(0.0f) {};
		explicit inline vec3(float v)
			: x(v), y(v), z(v) {};
		inline vec3(float x, float y, float z)
			: x(x), y(y), z(z) {};
		inline vec3(const vec2& xy, float z)
			: x(xy.x), y(xy.y), z(z) {};
		inline vec3(float x, const vec2& yz)
			: x(x), y(yz.x), z(yz.y) {};

		inline float Dot(const vec3& rhs) const
		{
			return x * rhs.x + y * rhs.y + z * rhs.z;
		};
		inline vec3 Cross(const vec3& rhs) const
		{
			return vec3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
		}

		inline float LengthSquared() const
		{
			return Dot(*this);
		};
		inline float Length() const
		{
			return std::sqrt(LengthSquared());
		};
		inline float Angle(const vec3& rhs) const
		{
			return std::acos(Dot(rhs) / (Length() * rhs.Length()));
		}

		inline bool IsZero() const
		{
			return x == 0.0f && y == 0.0f && z == 0.0f;
		}
		inline void Normalize()
		{
			*this /= Length();
		}
		inline vec3 GetNormalized() const
		{
			return *this / Length();
		}

		inline vec3& operator+=(const vec3& rhs)
		{
			this->x += rhs.x;
			this->y += rhs.y;
			this->z += rhs.z;
			return *this;
		};
		inline vec3& operator-=(const vec3& rhs)
		{
			this->x -= rhs.x;
			this->y -= rhs.y;
			this->z -= rhs.z;
			return *this;
		};
		inline vec3& operator*=(const vec3& rhs)
		{
			this->x *= rhs.x;
			this->y *= rhs.y;
			this->z *= rhs.z;
			return *this;
		};
		inline vec3& operator/=(const vec3& rhs)
		{
			this->x /= rhs.x;
			this->y /= rhs.y;
			this->z /= rhs.z;
			return *this;
		};
		inline vec3 operator+(const vec3& rhs) const
		{
			vec3 temp(*this);
			temp += rhs;
			return temp;
		};
		inline vec3 operator-(const vec3& rhs) const
		{
			vec3 temp(*this);
			temp -= rhs;
			return temp;
		};
		inline vec3 operator*(const vec3& rhs) const
		{
			vec3 temp(*this);
			temp *= rhs;
			return temp;
		};
		inline vec3 operator/(const vec3& rhs) const
		{
			vec3 temp(*this);
			temp /= rhs;
			return temp;
		};
		inline vec3 operator-() const
		{
			vec3 temp;
			temp -= *this;
			return temp;
		};
		inline vec3& operator*=(float rhs)
		{
			this->x *= rhs;
			this->y *= rhs;
			this->z *= rhs;
			return *this;
		};
		inline vec3& operator/=(float rhs)
		{
			this->x /= rhs;
			this->y /= rhs;
			this->z /= rhs;
			return *this;
		};
		inline vec3 operator*(float rhs) const
		{
			vec3 temp(*this);
			temp *= rhs;
			return temp;
		};
		inline vec3 operator/(float rhs) const
		{
			vec3 temp(*this);
			temp /= rhs;
			return temp;
		};
		inline bool operator==(const vec3& rhs) const
		{
			return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z;
		};
		inline float operator[](int i) const
		{
			Assert(i >= 0 && i < 3);
			if (i == 0) return x;
			if (i == 1) return y;
			if (i == 2) return z;
			return 0.0f;
		};
		inline float& operator[](int i)
		{
			Assert(i >= 0 && i < 3);
			if (i == 0) return x;
			if (i == 1) return y;
			if (i == 2) return z;
			return x;
		};
	};

	inline vec3 operator*(float lhs, const vec3& a)
	{
		return a * lhs; // assuming commutativity
	};

	class vec4
	{
	public:
		float x, y, z, w;

	public:
		inline vec4()
			: x(0.0f), y(0.0f), z(0.0f), w(0.0f) {};
		explicit inline vec4(float v)
			: x(v), y(v), z(v), w(v) {};
		inline vec4(float x, float y, float z, float w)
			: x(x), y(y), z(z), w(w) {};
		inline vec4(const vec2& xy, const vec2& zw)
			: x(xy.x), y(xy.y), z(zw.x), w(zw.y) {};
		inline vec4(const vec2& xy, float z, float w)
			: x(xy.x), y(xy.y), z(z), w(w) {};
		inline vec4(float x, float y, const vec2& zw)
			: x(x), y(y), z(zw.x), w(zw.y) {};
		inline vec4(const vec3& xyz, float w)
			: x(xyz.x), y(xyz.y), z(xyz.z), w(w) {};

		inline float Dot(const vec4& rhs) const
		{
			return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
		};

		inline float LengthSquared() const
		{
			return Dot(*this);
		};
		inline float Length() const
		{
			return std::sqrt(LengthSquared());
		};
		inline float Angle(const vec4& rhs) const
		{
			return std::acos(Dot(rhs) / (Length() * rhs.Length()));
		}

		inline bool IsZero() const
		{
			return x == 0.0f && y == 0.0f && z == 0.0f && w == 0.0f;
		}
		inline void Normalize()
		{
			*this /= Length();
		}
		inline vec4 GetNormalized() const
		{
			return *this / Length();
		}

		inline vec4& operator+=(const vec4& rhs)
		{
			this->x += rhs.x;
			this->y += rhs.y;
			this->z += rhs.z;
			this->w += rhs.w;
			return *this;
		};
		inline vec4& operator-=(const vec4& rhs)
		{
			this->x -= rhs.x;
			this->y -= rhs.y;
			this->z -= rhs.z;
			this->w -= rhs.w;
			return *this;
		};
		inline vec4& operator*=(const vec4& rhs)
		{
			this->x *= rhs.x;
			this->y *= rhs.y;
			this->z *= rhs.z;
			this->w *= rhs.w;
			return *this;
		};
		inline vec4& operator/=(const vec4& rhs)
		{
			this->x /= rhs.x;
			this->y /= rhs.y;
			this->z /= rhs.z;
			this->w /= rhs.w;
			return *this;
		};
		inline vec4 operator+(const vec4& rhs) const
		{
			vec4 temp(*this);
			temp += rhs;
			return temp;
		};
		inline vec4 operator-(const vec4& rhs) const
		{
			vec4 temp(*this);
			temp -= rhs;
			return temp;
		};
		inline vec4 operator*(const vec4& rhs) const
		{
			vec4 temp(*this);
			temp *= rhs;
			return temp;
		};
		inline vec4 operator/(const vec4& rhs) const
		{
			vec4 temp(*this);
			temp /= rhs;
			return temp;
		};
		inline vec4 operator-() const
		{
			vec4 temp;
			temp -= *this;
			return temp;
		};
		inline vec4& operator*=(float rhs)
		{
			this->x *= rhs;
			this->y *= rhs;
			this->z *= rhs;
			this->w *= rhs;
			return *this;
		};
		inline vec4& operator/=(float rhs)
		{
			this->x /= rhs;
			this->y /= rhs;
			this->z /= rhs;
			this->w /= rhs;
			return *this;
		};
		inline vec4 operator*(float rhs) const
		{
			vec4 temp(*this);
			temp *= rhs;
			return temp;
		};
		inline vec4 operator/(float rhs) const
		{
			vec4 temp(*this);
			temp /= rhs;
			return temp;
		};
		inline bool operator==(const vec4& rhs) const
		{
			return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z && this->w == rhs.w;
		};
		inline float operator[](int i) const
		{
			Assert(i >= 0 && i < 4);
			if (i == 0) return x;
			if (i == 1) return y;
			if (i == 2) return z;
			if (i == 3) return w;
			return 0.0f;
		};
		inline float& operator[](int i)
		{
			Assert(i >= 0 && i < 4);
			if (i == 0) return x;
			if (i == 1) return y;
			if (i == 2) return z;
			if (i == 3) return w;
			return x;
		};
	};

	inline vec4 operator*(float lhs, const vec4& a)
	{
		return a * lhs; // assuming commutativity
	};
}