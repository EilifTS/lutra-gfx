#include <lutra-gfx/Buffer.h>
#include "internal/BufferInternal.h"

namespace lgx
{
	Buffer::Buffer() {}

	Buffer::Buffer(GraphicsContext& ctx, u64 size, BufferType type)
	{
		vk::BufferUsageFlags usage_flags = vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst;

		if (type == BufferType::StorageBuffer)
		{
			usage_flags |= vk::BufferUsageFlagBits::eStorageBuffer;
		}

		internal = std::make_unique<BufferInternal>(*ctx.internal, size, usage_flags, 0);
	}

	Buffer::~Buffer() {}

	Buffer::Buffer(Buffer&&) = default;
	Buffer& Buffer::operator=(Buffer&&) = default;

	u64 Buffer::Size() const { return internal->Size(); }
	void* Buffer::Map(GraphicsContext& ctx) { return internal->Map(*ctx.internal->device); }
	void Buffer::Unmap(GraphicsContext& ctx) { internal->Unmap(*ctx.internal->device); }
}