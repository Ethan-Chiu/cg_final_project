//
//  StorageBuffer.cpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/28.
//

#include "ethpch.h"

#include "RendererAPI.h"
#include "StorageBuffer.h"

#include "Vulkan/VulkanStorageBuffer.h"

namespace Ethane {

    Ref<StorageBuffer> StorageBuffer::Create(uint32_t size)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None:
            ETH_CORE_ASSERT(false, "endererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::Vulkan:
            return CreateRef<VulkanStorageBuffer>(size);
        }

        ETH_CORE_ASSERT(false, "Unknpwn RendererAPI");
        return nullptr;
    }

    Ref<StorageBuffer> StorageBuffer::Create(void* data, uint32_t size)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None:
            ETH_CORE_ASSERT(false, "endererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::Vulkan:
            return CreateRef<VulkanStorageBuffer>(data, size);
        }

        ETH_CORE_ASSERT(false, "Unknpwn RendererAPI");
        return nullptr;
    }

}
