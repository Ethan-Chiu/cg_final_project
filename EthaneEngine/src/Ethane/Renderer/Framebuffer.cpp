#include "ethpch.h"
#include "Framebuffer.h"
#include "RendererAPI.h"
#include "Vulkan/vulkanFramebuffer.h"

namespace Ethane {

	Ref<Framebuffer> Framebuffer::Create(const GraphicsContext* ctx, const FramebufferSpecification& spec)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			ETH_CORE_ASSERT(false, "endererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanFramebuffer>(static_cast<const VulkanContext*>(ctx), spec);
		}

		ETH_CORE_ASSERT(false, "Unknpwn RendererAPI");
		return nullptr;
	}

}
