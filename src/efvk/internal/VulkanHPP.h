#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_TYPESAFE_CONVERSION 1
#include <vulkan/vulkan.hpp>

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include <vma/vk_mem_alloc.h>

namespace efvk
{
	/* Helpers */
	inline void change_layout(vk::CommandBuffer cmd_buf, vk::Image image, vk::ImageLayout old_layout, vk::ImageLayout new_layout)
	{
		const vk::ImageSubresourceRange range{
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		};

		const vk::ImageMemoryBarrier image_barrier{
			.srcAccessMask = vk::AccessFlagBits::eNone,
			.dstAccessMask = vk::AccessFlagBits::eNone,
			.oldLayout = old_layout,
			.newLayout = new_layout,
			.srcQueueFamilyIndex = vk::QueueFamilyIgnored,
			.dstQueueFamilyIndex = vk::QueueFamilyIgnored,
			.image = image,
			.subresourceRange = range,
		};

		cmd_buf.pipelineBarrier(
			vk::PipelineStageFlagBits::eAllCommands,
			vk::PipelineStageFlagBits::eAllCommands,
			vk::DependencyFlagBits::eByRegion,
			{},
			{},
			image_barrier
		);
	}

	/* VMA Wrappers */
	class VMABuffer
	{
	public:
		VMABuffer() {};
		~VMABuffer()
		{
			if (buffer != VK_NULL_HANDLE)
			{
				vmaDestroyBuffer(allocator, buffer, allocation);
			}
		}

		VMABuffer(const VMABuffer&) = delete;
		VMABuffer(VMABuffer&& rhs) noexcept
		{
			allocator = rhs.allocator;
			allocation = rhs.allocation;
			buffer = rhs.buffer;
			rhs.allocator = VK_NULL_HANDLE;
			rhs.allocation = VK_NULL_HANDLE;
			rhs.buffer = VK_NULL_HANDLE;
		}
		VMABuffer& operator=(const VMABuffer&) = delete;
		VMABuffer& operator=(VMABuffer&& rhs) noexcept
		{
			allocator = rhs.allocator;
			allocation = rhs.allocation;
			buffer = rhs.buffer;
			rhs.allocator = VK_NULL_HANDLE;
			rhs.allocation = VK_NULL_HANDLE;
			rhs.buffer = VK_NULL_HANDLE;
			return *this;
		}

		VmaAllocator GetAllocator() { return allocator; }
		VmaAllocation GetAllocation() { return allocation; }
		vk::Buffer GetBuffer() { return buffer; }

	private:
		VmaAllocator allocator{};
		VmaAllocation allocation{};
		VkBuffer buffer{};

		friend VMABuffer VMACreateBuffer(
			VmaAllocator allocator,
			const vk::BufferCreateInfo* pBufferCreateInfo,
			const VmaAllocationCreateInfo* pAllocationCreateInfo,
			VmaAllocationInfo* pAllocationInfo);
	};

	inline VMABuffer VMACreateBuffer(
		VmaAllocator allocator,
		const vk::BufferCreateInfo* pBufferCreateInfo,
		const VmaAllocationCreateInfo* pAllocationCreateInfo,
		VmaAllocationInfo* pAllocationInfo)
	{
		VMABuffer out_buffer{};
		out_buffer.allocator = allocator;
		const VkBufferCreateInfo vk_buf = *pBufferCreateInfo;
		VkResult result = vmaCreateBuffer(allocator, &vk_buf, pAllocationCreateInfo, &out_buffer.buffer, &out_buffer.allocation, pAllocationInfo);
		assert(result == VK_SUCCESS);
		return out_buffer;
	}

	class VMAImage
	{
	public:
		VMAImage() {};
		~VMAImage()
		{
			if (image != VK_NULL_HANDLE)
			{
				vmaDestroyImage(allocator, image, allocation);
			}
		}

		VMAImage(const VMAImage&) = delete;
		VMAImage(VMAImage&& rhs) noexcept
		{
			allocator = rhs.allocator;
			allocation = rhs.allocation;
			image = rhs.image;
			rhs.allocator = VK_NULL_HANDLE;
			rhs.allocation = VK_NULL_HANDLE;
			rhs.image = VK_NULL_HANDLE;
		}
		VMAImage& operator=(const VMAImage&) = delete;
		VMAImage& operator=(VMAImage&& rhs) noexcept
		{
			allocator = rhs.allocator;
			allocation = rhs.allocation;
			image = rhs.image;
			rhs.allocator = VK_NULL_HANDLE;
			rhs.allocation = VK_NULL_HANDLE;
			rhs.image = VK_NULL_HANDLE;
			return *this;
		}

		VmaAllocator GetAllocator() { return allocator; }
		VmaAllocation GetAllocation() { return allocation; }
		vk::Image GetImage() { return image; }

	private:
		VmaAllocator allocator{};
		VmaAllocation allocation{};
		VkImage image{};

		friend VMAImage VMACreateImage(
			VmaAllocator allocator,
			const vk::ImageCreateInfo* pImageCreateInfo,
			const VmaAllocationCreateInfo* pAllocationCreateInfo,
			VmaAllocationInfo* pAllocationInfo);
	};

	inline VMAImage VMACreateImage(
		VmaAllocator allocator,
		const vk::ImageCreateInfo* pImageCreateInfo,
		const VmaAllocationCreateInfo* pAllocationCreateInfo,
		VmaAllocationInfo* pAllocationInfo)
	{
		VMAImage out_image{};
		out_image.allocator = allocator;
		const VkImageCreateInfo vk_image = *pImageCreateInfo;
		VkResult result = vmaCreateImage(allocator, &vk_image, pAllocationCreateInfo, &out_image.image, &out_image.allocation, pAllocationInfo);
		assert(result == VK_SUCCESS);
		return out_image;
	}
}
