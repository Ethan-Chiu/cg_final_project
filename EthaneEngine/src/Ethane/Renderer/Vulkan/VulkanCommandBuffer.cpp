#include "ethpch.h"
#include "VulkanContext.h"
#include "VulkanCommandBuffer.h"


namespace Ethane {

    VulkanCommandBuffer::VulkanCommandBuffer(const VulkanDevice* device)
        :m_State(CommandBufferState::NOT_ALLOCATED), m_Device(device)
    {
    }

    void VulkanCommandBuffer::Allocate(VkCommandPool pool, bool isPrimary)
    {
        VkCommandBufferAllocateInfo cmdBufAllocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        cmdBufAllocateInfo.pNext = nullptr;
        cmdBufAllocateInfo.commandPool = pool;
        cmdBufAllocateInfo.level = isPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        cmdBufAllocateInfo.commandBufferCount = 1;

        m_State = CommandBufferState::NOT_ALLOCATED;
        VK_CHECK_RESULT(vkAllocateCommandBuffers(m_Device->GetVulkanDevice(), &cmdBufAllocateInfo, &m_CommandBuffer));
        m_State = CommandBufferState::READY;
    }

    void VulkanCommandBuffer::Free(VkCommandPool pool)
    {
        vkFreeCommandBuffers(m_Device->GetVulkanDevice(), pool, 1, &m_CommandBuffer);

        m_CommandBuffer = VK_NULL_HANDLE;
        m_State = CommandBufferState::NOT_ALLOCATED;
    }

    void VulkanCommandBuffer::Begin(bool isOneTimeUse, bool isRenderpassContinue, bool isSimultaneousUse) 
    {
        VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        beginInfo.flags = 0;
        if (isOneTimeUse) {
            beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        }
        if (isRenderpassContinue) {
            beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
        }
        if (isSimultaneousUse) {
            beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        }
        beginInfo.pInheritanceInfo = nullptr;
        VK_CHECK_RESULT(vkBeginCommandBuffer(m_CommandBuffer, &beginInfo));
        m_State = CommandBufferState::RECORDING;
    }

    void VulkanCommandBuffer::End() {
        VK_CHECK_RESULT(vkEndCommandBuffer(m_CommandBuffer));
        m_State = CommandBufferState::RECORDING_ENDED;
    }

    void VulkanCommandBuffer::UpdateSubmitted() 
    {
        m_State = CommandBufferState::SUBMITTED;
    }

    void VulkanCommandBuffer::Reset() {
        m_State = CommandBufferState::READY;
    }

    void VulkanCommandBuffer::AllocateAndBeginSingleUse(VkCommandPool pool)
    {
        Allocate(pool, true);
        Begin(true, false, false);
    }

    void VulkanCommandBuffer::EndSingleUse(VkCommandPool pool, VkQueue queue) 
    {
        End();

        // Submit the queue
        VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &m_CommandBuffer;
        VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submit_info, 0));

        // Wait for it to finish
        VK_CHECK_RESULT(vkQueueWaitIdle(queue));

        // Free the command buffer.
        Free(pool);
    }

    void VulkanCommandBuffer::AllocateAndBeginSingleUse(QueueFamilyTypes type)
    {
        VkCommandPool pool;
        switch (type)
        {
        case QueueFamilyTypes::Graphics: {pool = m_Device->GetGraphicsCommandPool(); break; }
        case QueueFamilyTypes::Compute: {pool = m_Device->GetComputeCommandPool(); break; }
        default:
            ETH_CORE_ASSERT(false, "No command pool");
        }
        AllocateAndBeginSingleUse(pool);
    }

    void VulkanCommandBuffer::EndSingleUse(QueueFamilyTypes type)
    {
        VkCommandPool pool;
        VkQueue queue;
        switch (type)
        {
        case QueueFamilyTypes::Graphics: 
        {
            pool = m_Device->GetGraphicsCommandPool();
            queue = m_Device->GetGraphicsQueue();
            break; 
        }
        case QueueFamilyTypes::Compute: 
        {
            pool = m_Device->GetComputeCommandPool();
            queue = m_Device->GetComputeQueue();
            break; 
        }
        default:
            ETH_CORE_ASSERT(false, "No command pool");
        }

        End();

        // Submit the queue
        VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &m_CommandBuffer;
        VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submit_info, 0));

        // Wait for it to finish
        VK_CHECK_RESULT(vkQueueWaitIdle(queue));

        // Free the command buffer.
        Free(pool);
    }

}
