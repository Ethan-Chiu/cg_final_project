//
//  VulkanStorageBuffer.cpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/28.
//

#include "ethpch.h"
#include "VulkanStorageBuffer.h"


namespace Ethane {
        
    VulkanStorageBuffer::VulkanStorageBuffer(uint32_t size)
        :VulkanBuffer()
    {
        m_Size = size;
        CreateVulkanBuffer(m_Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true);
    }

    VulkanStorageBuffer::VulkanStorageBuffer(void* data, uint32_t size)
        :VulkanBuffer()
    {
        m_Size = size;
        
        // create vertex buffer (gpu local memory)
        CreateVulkanBuffer(m_Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true);

        SetData(data, size);
    }

    void VulkanStorageBuffer::Destroy()
    {
        VulkanBuffer::Destroy();
    }


    void VulkanStorageBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
    {
        // create staging buffer
        VulkanBuffer stagingBuffer{};
        stagingBuffer.CreateVulkanBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true);

        // copy data to staging buffer
        stagingBuffer.SetData(data, offset, size, 0, 0);

        // copy from staging buffer
        VulkanBuffer::CopyTo(m_Device, m_Device->GetGraphicsCommandPool(), 0, m_Device->GetGraphicsQueue(), stagingBuffer.GetHandle(), 0, m_Buffer, 0, size);

        // cleanup staging buffer
        stagingBuffer.Destroy();
    }

    VkDescriptorBufferInfo VulkanStorageBuffer::GetBufferInfo() const
    {
        VkDescriptorBufferInfo bufferInfo;
        bufferInfo.buffer = m_Buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = m_Size;
        return bufferInfo;

    }

}
