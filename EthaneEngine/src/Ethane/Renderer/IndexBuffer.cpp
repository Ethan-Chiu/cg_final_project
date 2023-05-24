#include "ethpch.h"
#include "IndexBuffer.h"

#include "RendererAPI.h"

#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanIndexBuffer.h"


namespace Ethane {

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			ETH_CORE_ASSERT(false, "endererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanIndexBuffer>(indices, size);
		}

		ETH_CORE_ASSERT(false, "Unknpwn RendererAPI");
		return nullptr;
	}

}
