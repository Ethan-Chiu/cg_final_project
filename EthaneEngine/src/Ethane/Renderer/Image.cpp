#include "ethpch.h"
#include "Image.h"

#include "Ethane/Renderer/RendererAPI.h"

//#include "Vulkan/VulkanImage.h"

namespace Ethane{

	Ref<Image2D> Image2D::Create(ImageSpecification specification, void* data)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: return nullptr;
		// case RendererAPI::API::OpenGL: return Ref<OpenGLImage2D>::Create(specification, data);
//		case RendererAPI::API::Vulkan: return CreateRef<VulkanImage2D>(specification);
        case RendererAPI::API::Vulkan: return nullptr;
		}
		ETH_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}
