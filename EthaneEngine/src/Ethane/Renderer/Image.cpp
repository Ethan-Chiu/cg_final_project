#include "ethpch.h"
#include "Image.h"

#include "RendererAPI.h"

#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanImage.h"
#include "Vulkan/VulkanTargetImage.h"

namespace Ethane{

	Ref<Image2D> Image2D::Create(ImageSpecification specification, void* data, uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: return nullptr;
            case RendererAPI::API::Vulkan: return CreateRef<VulkanImage2D>(specification, data, size);
		}
		ETH_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}


    Ref<TargetImage> TargetImage::Create(ImageSpecification specification)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None: return nullptr;
            case RendererAPI::API::Vulkan: return CreateRef<VulkanTargetImage>(specification);
        }
        ETH_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
}
