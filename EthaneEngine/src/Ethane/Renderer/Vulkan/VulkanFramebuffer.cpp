#include "ethpch.h"
#include "VulkanFramebuffer.h"

#include "Ethane/Renderer/Renderer.h"

#include "VulkanContext.h"

namespace Ethane {

	VulkanFramebuffer::VulkanFramebuffer(const VulkanDevice* device, const VulkanRenderPass* renderpass)
		: m_Device(device), m_RenderPass(renderpass)
	{
	}

    void VulkanFramebuffer::Destroy()
    {
        auto device = m_Device->GetVulkanDevice();
        
        if (m_Framebuffer)
        {
            vkDeviceWaitIdle(device);
            vkDestroyFramebuffer(device, m_Framebuffer, nullptr);
        }
        m_Framebuffer = nullptr;
    }
    
	void VulkanFramebuffer::Invalidate(uint32_t width, uint32_t height, const std::vector<VkImageView>& imageViews)
	{
		auto device = m_Device->GetVulkanDevice();

        // TODO: change this: investigate resources management( release at appropriate time )
        Destroy();
        
		// Create Framebuffer
		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = m_RenderPass->GetHandle();
		framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(imageViews.size());
		framebufferCreateInfo.pAttachments = imageViews.data();
		framebufferCreateInfo.width = width;
		framebufferCreateInfo.height = height;
		framebufferCreateInfo.layers = 1;

		ETH_CORE_TRACE("Framebuffer with {0} attachmment created", uint32_t(imageViews.size()));
		VK_CHECK_RESULT(vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &m_Framebuffer));
	}

}
