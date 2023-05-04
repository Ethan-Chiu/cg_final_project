#include "ethpch.h"
#include "RenderPass.h"

#include "RendererAPI.h"

#include "Vulkan/VulkanRenderPass.h"

namespace Ethane {
	Ref<RenderPass> RenderPass::Create()
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			ETH_CORE_ASSERT(false, "endererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanRenderPass>();
		}

		ETH_CORE_ASSERT(false, "Unknpwn RendererAPI");
		return nullptr;
	}
}
