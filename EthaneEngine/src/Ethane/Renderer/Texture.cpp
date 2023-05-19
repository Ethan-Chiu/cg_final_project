#include "ethpch.h"
#include "Texture.h"

#include "RendererAPI.h"
#include "Vulkan/VulkanTexture.h"

namespace Ethane {

	Ref<Texture2D> Texture2D::Create(const GraphicsContext* ctx, Image2D* image)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			ETH_CORE_ASSERT(false, "endererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanTexture2D>(static_cast<const VulkanContext*>(ctx), static_cast<VulkanImage2D*>(image));
		}

		ETH_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

//	Ref<Texture2D> Texture2D::Create(const std::string& path)
//	{
//		switch (RendererAPI::GetAPI())
//		{
//		case RendererAPI::API::None:
//			ETH_CORE_ASSERT(false, "endererAPI::None is currently not supported!");
//			return nullptr;
//		case RendererAPI::API::Vulkan:
//			return CreateRef<VulkanTexture2D>(path);
//		}
//
//		ETH_CORE_ASSERT(false, "Unknown RendererAPI");
//		return nullptr;
//	}

}
