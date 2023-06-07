#include "ethpch.h"
#include "RenderCommandBuffer.h"

#include "Vulkan/VulkanRenderCommandBuffer.h"

#include "RendererAPI.h"

namespace Ethane {

	Ref<RenderCommandBuffer> RenderCommandBuffer::Create(bool isInFlight, const std::string& debugName)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:    return nullptr;
			case RendererAPI::API::Vulkan:  return CreateRef<VulkanRenderCommandBuffer>(isInFlight, debugName);
		}
		ETH_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}
