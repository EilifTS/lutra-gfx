#pragma once
#include <vector>
#include <lutra-gfx/CommonStructs.h>
#include "VulkanHPP.h"

namespace lgx
{
	/* TODO: Remove allocations from this class */
	class DescriptorWriteCache
	{
	public:
		void AddBufferWrite(u32 binding, vk::Buffer buffer, u64 buffer_size)
		{
			write_infos.push_back({
				.binding = binding,
				.additional_info_index = static_cast<u32>(buffer_write_infos.size()),
				.descriptor_count = 1,
				.write_type = vk::DescriptorType::eStorageBuffer,
			});

			buffer_write_infos.push_back({
				.buffer = buffer,
				.range = buffer_size,
			});
			is_dirty = true;
		};

		void AddImageWrite(u32 binding, vk::ImageView view)
		{
			write_infos.push_back({
				.binding = binding,
				.additional_info_index = static_cast<u32>(image_write_infos.size()),
				.descriptor_count = 1,
				.write_type = vk::DescriptorType::eSampledImage,
			});
			image_write_infos.push_back({
				.imageView = view,
				.imageLayout = vk::ImageLayout::eGeneral,
			});
			is_dirty = true;
		};

		void AddImageArrayWrite(u32 binding, std::span<vk::ImageView> views)
		{
			write_infos.push_back({
				.binding = binding,
				.additional_info_index = static_cast<u32>(image_write_infos.size()),
				.descriptor_count = static_cast<u32>(views.size()),
				.write_type = vk::DescriptorType::eSampledImage,
			});
			for (u32 i = 0; i < static_cast<u32>(views.size()); i++)
			{
				image_write_infos.push_back({
					.imageView = views[i],
					.imageLayout = vk::ImageLayout::eGeneral,
				});
			}
			is_dirty = true;
		};

		bool IsDirty() const { return is_dirty; };

		void Flush(vk::Device dev, vk::DescriptorSet set);
		void Clear()
		{
			write_infos.clear();
			buffer_write_infos.clear();
			image_write_infos.clear();
			is_dirty = false;
		}

	private:
		struct WriteInfo
		{
			u32 binding{};
			u32 additional_info_index{};
			u32 descriptor_count{};
			vk::DescriptorType write_type{};
		};
		std::vector<WriteInfo> write_infos{};
		std::vector<vk::DescriptorBufferInfo> buffer_write_infos{};
		std::vector<vk::DescriptorImageInfo> image_write_infos{};

		bool is_dirty{ false };
	};
}