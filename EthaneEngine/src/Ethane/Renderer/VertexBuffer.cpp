#include "ethpch.h"
#include "RendererAPI.h"
#include "VertexBuffer.h"

#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanVertexBuffer.h"

namespace Ethane {

	Ref<VertexBuffer> VertexBuffer::Create(const GraphicsContext* ctx, uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: 
				ETH_CORE_ASSERT(false, "endererAPI::None is currently not supported!");
				return nullptr;
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanVertexBuffer>(static_cast<const VulkanContext*>(ctx), size);
		}

		ETH_CORE_ASSERT(false, "Unknpwn RendererAPI");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(const GraphicsContext* ctx, void* vertices, uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			ETH_CORE_ASSERT(false, "endererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanVertexBuffer>(static_cast<const VulkanContext*>(ctx), vertices, size);
		}

		ETH_CORE_ASSERT(false, "Unknpwn RendererAPI");
		return nullptr;
	}

}
