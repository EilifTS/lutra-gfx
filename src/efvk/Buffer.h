#pragma once
#include <efvk/GraphicsContext.h>
#include <efvk/math/IntDefs.h>
#include "GraphicsContextImpl.h"
#include "VulkanHPP.h"

namespace efvk
{
	class Buffer
	{
	public:
		Buffer() {};
		Buffer(GraphicsContext::Impl& ctx, u64 size, vk::BufferUsageFlags usage, VmaAllocationCreateFlags vma_flags);

		Buffer(const Buffer&) = delete;
		Buffer(Buffer&& rhs) noexcept = default;
		Buffer& operator=(const Buffer&) = delete;
		Buffer& operator=(Buffer&& rhs) noexcept = default;

		u64 Size() const { return size; }
		void* Map(vk::Device device);
		void Unmap(vk::Device device);

		VMABuffer buffer;

	private:
		u64 size{};
	};
}