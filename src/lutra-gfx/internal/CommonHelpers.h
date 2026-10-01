#pragma once
#include <lutra-gfx/CommonStructs.h>
#include <assert.h>
#include "VulkanHPP.h"

namespace lgx
{
	inline vk::Format convert_color_format(ColorFormat format)
	{
		switch (format)
		{
		case ColorFormat::RGBA8: return vk::Format::eR8G8B8A8Unorm;
		case ColorFormat::BGRA8: return vk::Format::eB8G8R8A8Unorm;
		}
		assert(false);
		return vk::Format::eUndefined;
	}

	inline vk::Format convert_ds_format(DepthStencilFormat format)
	{
		switch (format)
		{
		case DepthStencilFormat::D32: return vk::Format::eD32Sfloat;
		default: assert(false);
		}
		assert(false);
		return vk::Format::eUndefined;
	}
}