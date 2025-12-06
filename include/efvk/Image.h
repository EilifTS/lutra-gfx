#pragma once
#include <vector>
#include <efvk/math/IntDefs.h>

namespace efvk
{
	class Image
	{
	public:
		Image() {};
		Image(u32 width, u32 height) : width(width), height(height), data(4 * width * height) {}

		void Load(const char* filename);

		u32 Width() const { return width; }
		u32 Height() const { return height; }
		const char* GetDataPtr() const { return data.data(); }
		char GetData(u32 i) const { return data[i]; }
		void SetData(u32 i, const char v) { data[i] = v; }

	private:
		u32 width{};
		u32 height{};
		std::vector<char> data{};
	};
}