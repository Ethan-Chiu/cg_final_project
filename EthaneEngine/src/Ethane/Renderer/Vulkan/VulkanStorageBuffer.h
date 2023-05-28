//
//  VulkanStorageBuffer.h
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/28.
//

#pragma once

#include "Ethane/Renderer/StorageBuffer.h"
#include "VulkanBuffer.h"

namespace Ethane {

    class VulkanStorageBuffer : public StorageBuffer, public VulkanBuffer
    {
    public:
        VulkanStorageBuffer(uint32_t size); // size in byte
        VulkanStorageBuffer(void* data, uint32_t size = 0);
        virtual ~VulkanStorageBuffer() {};

        void Destroy() override;

        void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;

        //Getter
        uint32_t GetSize() const { return m_Size; }
        VkDescriptorBufferInfo GetBufferInfo() const;
        
    private:
    };

}
