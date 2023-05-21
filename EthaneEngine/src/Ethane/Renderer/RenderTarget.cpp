//
//  RenderTarget.cpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/20.
//

#include "RendererAPI.h"
#include "RenderTarget.h"

#include "Vulkan/VulkanRenderTarget.h"

namespace Ethane {
Scope<RenderTarget> RenderTarget::Create(const GraphicsContext* ctx, const RenderTargetSpecification& spec)
{
    switch (RendererAPI::GetAPI())
    {
    case RendererAPI::API::None:
        ETH_CORE_ASSERT(false, "endererAPI::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::Vulkan:
        return CreateScope<VulkanRenderTarget>(static_cast<const VulkanContext*>(ctx), spec);
    }

    ETH_CORE_ASSERT(false, "Unknpwn RendererAPI");
    return nullptr;
}
}
