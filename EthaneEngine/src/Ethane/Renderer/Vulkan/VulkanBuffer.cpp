#include "ethpch.h"
#include "VulkanBuffer.h"

#include "VulkanUtils.h"
#include "VulkanCommandBuffer.h"

namespace Ethane {

    VulkanBuffer::VulkanBuffer()
        :m_Device(VulkanContext::GetDevice())
    {
    }

    bool VulkanBuffer::CreateVulkanBuffer(
        uint32_t size, 
        VkBufferUsageFlags usage,
        uint32_t memory_property_flags,
        bool bind_on_create) {

        m_Size = size;
        m_Usage = usage;
        m_MemoryFlag = memory_property_flags;

        VkBufferCreateInfo bufferInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        
        auto vk_device = m_Device->GetVulkanDevice();

        VK_CHECK_RESULT(vkCreateBuffer(vk_device, &bufferInfo, nullptr, &m_Buffer));

        // get memory requirements
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(vk_device, m_Buffer, &memRequirements);

        // allocate memory
        m_MemoryIndex = Utils::FindMemoryType(m_Device->GetPhysicalDevice()->GetVulkanPhysicalDevice(), memRequirements.memoryTypeBits, m_MemoryFlag); // TODO: handle can't find
        VkMemoryAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = m_MemoryIndex;

        if (vkAllocateMemory(vk_device, &allocInfo, nullptr, &m_Memory) != VK_SUCCESS) {
            ETH_CORE_ERROR("failed to allocate buffer memory!");
            // clean up
            return false;
        }

        if (bind_on_create) {
            VK_CHECK_RESULT(vkBindBufferMemory(vk_device, m_Buffer, m_Memory, 0));
        }
        return true;
    }

    void VulkanBuffer::Destroy() {
        auto device = m_Device->GetVulkanDevice();
        if (m_Memory) {
            vkFreeMemory(device, m_Memory, nullptr);
            m_Memory = nullptr;
        }
        if (m_Buffer) {
            vkDestroyBuffer(device, m_Buffer, nullptr);
            m_Buffer = nullptr;
        }
        
        m_Size = 0;
        // buffer->is_locked = false;
    }

    bool VulkanBuffer::Resize(
        uint32_t new_size,
        VkQueue queue,
        VkCommandPool pool) {

        // Sanity check.
        if (new_size < m_Size) {
            ETH_CORE_ERROR("vulkan_buffer_resize requires that new size be larger than the old. Not doing this could lead to data loss.");
            return false;
        }

        m_Size = new_size;

        // Create new buffer.
        VkBufferCreateInfo bufferInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufferInfo.size = m_Size;
        bufferInfo.usage = m_Usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        auto device = m_Device->GetVulkanDevice();

        VkBuffer new_buffer;
        VK_CHECK_RESULT(vkCreateBuffer(device, &bufferInfo, nullptr, &new_buffer));

        // Get memory requirements
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, new_buffer, &memRequirements);

        // Allocate memory
        VkMemoryAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = m_MemoryIndex;

        VkDeviceMemory new_memory;
        if (vkAllocateMemory(device, &allocInfo, nullptr, &new_memory) != VK_SUCCESS) {
            ETH_CORE_ERROR("failed to allocate buffer memory during resize!");
            // clean up
            return false;
        }

        // Bind the new buffer's memory
        VK_CHECK_RESULT(vkBindBufferMemory(device, new_buffer, new_memory, 0));

        // Copy over the data
        CopyTo(m_Device, pool, 0, queue, m_Buffer, 0, new_buffer, 0, m_Size);

        // Make sure anything potentially using these is finished.
        vkDeviceWaitIdle(device);

        // Destroy the old
        if (m_Memory) {
            vkFreeMemory(device, m_Memory, nullptr);
            m_Memory = nullptr;
        }
        if (m_Buffer) {
            vkDestroyBuffer(device, m_Buffer, nullptr);
            m_Buffer = nullptr;
        }

        // Set new properties
        m_Buffer = new_buffer;
        m_Memory = new_memory;

        return true;
    }

    void VulkanBuffer::Bind(uint32_t offset) {
        auto device = m_Device->GetVulkanDevice();
        VK_CHECK_RESULT(vkBindBufferMemory(device, m_Buffer, m_Memory, offset));
    }

    void* VulkanBuffer::LockMemory(uint32_t offset, uint32_t size, uint32_t flags) {
        auto device = m_Device->GetVulkanDevice();

        void* data;
        VK_CHECK_RESULT(vkMapMemory(device, m_Memory, offset, size, flags, &data));
        return data;
    }

    void VulkanBuffer::UnlockMemory() {
        auto device = m_Device->GetVulkanDevice();
        vkUnmapMemory(device, m_Memory);
    }

    void VulkanBuffer::SetData(const void* data, uint32_t srcOffset, uint32_t size, uint32_t dstOffset, uint32_t flags) {
        auto device = m_Device->GetVulkanDevice();

        void* dstData;
        VK_CHECK_RESULT(vkMapMemory(device, m_Memory, dstOffset, size, flags, &dstData));
        memcpy(dstData, (uint8_t*)data + srcOffset, size); // add offset or not
        vkUnmapMemory(device, m_Memory);
    }

    void VulkanBuffer::CopyTo(
        const VulkanDevice* device,
        VkCommandPool pool,
        VkFence fence,
        VkQueue queue,
        VkBuffer source,
        uint32_t source_offset,
        VkBuffer dest,
        uint32_t dest_offset,
        uint32_t size) {

        // create command buffer
        VulkanCommandBuffer tempCommandBuf{device};
        tempCommandBuf.AllocateAndBeginSingleUse(pool);

        // record command
        VkBufferCopy copyRegion;
        copyRegion.srcOffset = source_offset;
        copyRegion.dstOffset = dest_offset;
        copyRegion.size = size;
        vkCmdCopyBuffer(tempCommandBuf.GetHandle(), source, dest, 1, &copyRegion);

        // submit command buffer and wait for complete
        tempCommandBuf.EndSingleUse(pool, queue);
    }
}
