#include <efvk/CommonStructs.h>
#include <assert.h>
#include "VulkanHPP.h"

namespace efvk
{
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