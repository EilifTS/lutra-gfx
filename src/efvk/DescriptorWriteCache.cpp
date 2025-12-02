#include "DescriptorWriteCache.h"

namespace efvk
{
	static constexpr u32 max_writes = 16;

	void DescriptorWriteCache::Flush(vk::Device dev, vk::DescriptorSet set)
	{
		assert(is_dirty);

		std::array<vk::WriteDescriptorSet, max_writes> writes{};
		std::array<vk::DescriptorBufferInfo, max_writes> buffer_infos{};
		std::array<vk::DescriptorImageInfo, max_writes> image_infos{};

		u32 write_index = 0;
		for (const BufferWriteInfo& buffer_write_info : buffer_write_infos)
		{
			buffer_infos[write_index] = {
				.buffer = buffer_write_info.buffer,
				.range = buffer_write_info.buffer_size,
			};

			writes[write_index] = {
				.dstSet = set,
				.dstBinding = buffer_write_info.binding,
				.descriptorCount = 1,
				.descriptorType = vk::DescriptorType::eStorageBuffer,
				.pBufferInfo = &buffer_infos[write_index],
			};

			write_index++;

			if (write_index == max_writes)
			{
				dev.updateDescriptorSets(write_index, writes.data(), 0, {});
				Clear();
				write_index = 0;
			}
		}

		for (const ImageWriteInfo& image_write_info : image_write_infos)
		{
			image_infos[write_index] = {
				.imageView = image_write_info.image_view,
				.imageLayout = vk::ImageLayout::eGeneral,
			};

			writes[write_index] = {
				.dstSet = set,
				.dstBinding = image_write_info.binding,
				.descriptorCount = 1,
				.descriptorType = vk::DescriptorType::eSampledImage,
				.pImageInfo = &image_infos[write_index],
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