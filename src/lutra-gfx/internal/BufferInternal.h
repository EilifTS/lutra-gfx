#pragma once
#include <lutra-gfx/CommonStructs.h>
#include <lutra-gfx/GraphicsContext.h>
#include "GraphicsContextInternal.h"
#include "VulkanHPP.h"

namespace lgx
{
	class BufferInternal
	{
	public:
		BufferInternal() {};
		BufferInternal(GraphicsContextInternal& ctx, u64 size, vk::BufferUsageFlags usage, VmaAllocationCreateFlags vma_flags);

		BufferInternal(const BufferInternal&) = delete;
		BufferInternal(BufferInternal&& rhs) noexcept = default;
		BufferInternal& operator=(const BufferInternal&) = delete;
		BufferInternal& operator=(BufferInternal&& rhs) noexcept = default;

		u64 Size() const { return size; }
		void* Map(vk::Device device);
		void Unmap(vk::Device device);

		VMABuffer buffer;

	private:
		u64 size{};
	};
}