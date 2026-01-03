#include "DescriptorWriteCache.h"

namespace lgx
{
	static constexpr u32 max_writes = 16;

	void DescriptorWriteCache::Flush(vk::Device dev, vk::DescriptorSet set)
	{
		assert(is_dirty);

		std::array<vk::WriteDescriptorSet, max_writes> writes{};

		u32 write_index = 0;
		for (const WriteInfo& write_info : write_infos)
		{
			const vk::DescriptorBufferInfo* buffer_info_ptr = nullptr;
			const vk::DescriptorImageInfo* image_info_ptr = nullptr;
			if (write_info.write_type == vk::DescriptorType::eStorageBuffer)
			{
				buffer_info_ptr = &buffer_write_infos[write_info.additional_info_index];
			}
			else
			{
				image_info_ptr = &image_write_infos[write_info.additional_info_index];
			}

			writes[write_index] = {
				.dstSet = set,
				.dstBinding = write_info.binding,
				.descriptorCount = write_info.descriptor_count,
				.descriptorType = write_info.write_type,
				.pImageInfo = image_info_ptr,
				.pBufferInfo = buffer_info_ptr,
			};

			write_index++;

			if (write_index == max_writes)
			{
				dev.updateDescriptorSets(write_index, writes.data(), 0, {});
				Clear();
				write_index = 0;
			}
		}

		dev.updateDescriptorSets(write_index, writes.data(), 0, {});
		is_dirty = false;
	}
}