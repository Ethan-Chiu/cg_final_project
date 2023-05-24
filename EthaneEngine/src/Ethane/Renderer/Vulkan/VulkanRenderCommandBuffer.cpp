#include "ethpch.h"
#include "VulkanRenderCommandBuffer.h"
#include "VulkanSwapChain.h"
#include <utility>


namespace Ethane {

    VulkanRenderCommandBuffer::VulkanRenderCommandBuffer(uint32_t count, std::string debugName)
		: m_DebugName(std::move(debugName))
	{
        uint32_t framesInFlight = VulkanContext::GetFramesInFlight();
        const VulkanDevice* device = VulkanContext::GetDevice();

		
		VkCommandPoolCreateInfo cmdPoolInfo = {};
		cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmdPoolInfo.queueFamilyIndex = VulkanContext::GetPhysicalDevice()->GetQueueFamilyIndices().Graphics.value();
		cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		VK_CHECK_RESULT(vkCreateCommandPool(device->GetVulkanDevice(), &cmdPoolInfo, nullptr, &m_CommandPool));

        if (count == 0)
            count = framesInFlight;
        m_CommandBuffers.resize(count, VulkanCommandBuffer(device));
        for (auto& cmdBuffer : m_CommandBuffers)
        {
            cmdBuffer.Allocate(m_CommandPool, true);
        }
        
        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        m_WaitFences.resize(framesInFlight);
        for (auto& fence : m_WaitFences)
            VK_CHECK_RESULT(vkCreateFence(device->GetVulkanDevice(), &fenceCreateInfo, nullptr, &fence));
	}

    void VulkanRenderCommandBuffer::Destroy()
    {
        auto device = VulkanContext::GetDevice()->GetVulkanDevice();
        
        vkDeviceWaitIdle(device);
        for (auto& fence : m_WaitFences)
            vkDestroyFence(device, fence, nullptr);
    
        VkCommandPool commandPool = m_CommandPool;
        vkDestroyCommandPool(device, commandPool, nullptr);
    }

	void VulkanRenderCommandBuffer::Begin()
	{
		uint32_t frameIndex = VulkanContext::GetSwapchain()->GetCurrentFrameIndex();
		m_CommandBuffers[frameIndex].Begin(true, false, false);
	}

	void VulkanRenderCommandBuffer::End()
	{
		uint32_t frameIndex = VulkanContext::GetSwapchain()->GetCurrentFrameIndex();
        m_CommandBuffers[frameIndex].End();
	}

	void VulkanRenderCommandBuffer::Submit()
	{
        uint32_t frameIndex = VulkanContext::GetSwapchain()->GetCurrentFrameIndex();

        auto device = VulkanContext::GetDevice()->GetVulkanDevice();
        
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        VkCommandBuffer commandBuffer = m_CommandBuffers[frameIndex].GetHandle();
        submitInfo.pCommandBuffers = &commandBuffer;
        
        VK_CHECK_RESULT(vkWaitForFences(device, 1, &m_WaitFences[frameIndex], VK_TRUE, UINT64_MAX));
        VK_CHECK_RESULT(vkResetFences(device, 1, &m_WaitFences[frameIndex]));
        VK_CHECK_RESULT(vkQueueSubmit(VulkanContext::GetDevice()->GetGraphicsQueue(), 1, &submitInfo, m_WaitFences[frameIndex]));
	}

}
