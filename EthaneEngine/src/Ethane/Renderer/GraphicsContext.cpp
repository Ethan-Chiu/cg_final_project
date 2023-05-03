#include "ethpch.h"
#include "GraphicsContext.h"
#include "RendererAPI.h"
#include "Vulkan/VulkanContext.h"

namespace Ethane {

	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    ETH_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::Vulkan:  return CreateScope<VulkanContext>(static_cast<GLFWwindow*>(window));
		}

		ETH_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}