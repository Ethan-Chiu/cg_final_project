#include "ethpch.h"
#include "Image.h"

#include "RendererAPI.h"

#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanImage.h"

namespace Ethane{

	Ref<Image2D> Image2D::Create(const GraphicsContext* ctx, ImageSpecification specification, void* data)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: return nullptr;
            case RendererAPI::API::Vulkan: return CreateRef<VulkanImage2D>(static_cast<const VulkanContext*>(ctx), specification);
		}
		ETH_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}
