#pragma once
#include <vector>

#include "CommonStructs.h"
#include "GraphicsContext.h"

namespace lgx
{
	class BufferInternal;

	enum class BufferType
	{
		StorageBuffer,
	};

	class Buffer
	{
	public:
		Buffer();
		Buffer(GraphicsContext& ctx, u64 size, BufferType type);
		~Buffer();

		Buffer(const Buffer&) = delete;
		Buffer(Buffer&&);
		Buffer& operator=(const Buffer&) = delete;
		Buffer& operator=(Buffer&&);

		std::unique_ptr<BufferInternal> internal{};

		u64 Size() const;
		void* Map(GraphicsContext& ctx);
		void Unmap(GraphicsContext& ctx);
	};
}