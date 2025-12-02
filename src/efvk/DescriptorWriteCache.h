#pragma once
#include <vector>
#include <efvk/math/IntDefs.h>
#include "VulkanHPP.h"

namespace efvk
{
	/* TODO: Remove allocations from this class */
	class DescriptorWriteCache
	{
	public:
		void AddBufferWrite(u32 binding, vk::Buffer buffer, u64 buffer_size)
		{
			buffer_write_infos.push_back({ binding, buffer, buffer_size });
			is_dirty = true;
		};

		void AddImageWrite(u32 binding, vk::ImageView view)
		{
			image_write_infos.push_back({ binding, view });
			is_dirty = true;
		};

		bool IsDirty() const { return is_dirty; };

		void Flush(vk::Device dev, vk::DescriptorSet set);
		void Clear()
		{
			buffer_write_infos.clear();
			image_write_infos.clear();
			is_dirty = false;
		}

	private:
		struct BufferWriteInfo
		{
			u32 binding{};
			vk::Buffer buffer{};
			u64 buffer_size{};
		};

		std::vector<BufferWriteInfo> buffer_write_infos{};

		struct ImageWriteInfo
		{
			u32 binding{};
			vk::ImageView image_view{};
		};

		std::vector<ImageWriteInfo> image_write_infos{};

		bool is_dirty{ false };
	};
}