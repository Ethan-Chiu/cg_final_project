#include "ethpch.h"
#include "VulkanFramebuffer.h"

#include "Ethane/Renderer/Renderer.h"

#include "VulkanContext.h"

namespace Ethane {

	VulkanFramebuffer::VulkanFramebuffer(const VulkanContext* ctx, const FramebufferSpecification& specification)
		: m_Specification(specification), m_Context(ctx)
	{
		Resize(specification.Width, specification.Height);
	}


	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		 if (m_Width == width && m_Height == height)
		 	return;

		m_Width = width;
		m_Height = height;
		
        Invalidate();
	}

    void VulkanFramebuffer::Destroy()
    {
        auto device = m_Context->GetDevice()->GetVulkanDevice();
        
        if (m_Framebuffer)
        {
            vkDeviceWaitIdle(device);
            VkFramebuffer framebuffer = m_Framebuffer;
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }
        m_Framebuffer = nullptr;
    }
    
	void VulkanFramebuffer::Invalidate()
	{
		ETH_CORE_TRACE("VulkanFramebuffer::Invalidate ({})", m_Specification.DebugName);

		auto device = m_Context->GetDevice()->GetVulkanDevice();

		if (m_Framebuffer)
		{
			// TODO: change this: investigate resources management( release at appropriate time )
			vkDeviceWaitIdle(device);
			vkDestroyFramebuffer(device, m_Framebuffer, nullptr);
            
			vkDestroyRenderPass(device, m_RenderPass->GetHandle(), nullptr);

            for(auto target: m_AttachmentImages)
            {
                target->Resize(m_Width, m_Height);
            }
		}

		std::vector<VkAttachmentDescription> attachmentDescriptions;
		std::vector<VkAttachmentReference> colorAttachmentReferences;
		VkAttachmentReference depthAttachmentReference;

        m_ClearValues.resize(m_Specification.Attachments.Attachments.size());

		uint32_t attachmentIndex = 0;
		for (auto attachment : m_Specification.Attachments.Attachments)
		{
			// Depth attachment
            auto imageFormat = attachment.AttachImage->GetSpecification().Format;
			if (Utils::IsDepthFormat(imageFormat))
			{
                m_DepthAttachmentImage = static_cast<VulkanImage2D*>(attachment.AttachImage);
                m_DepthAttachmentFormat = imageFormat;
				m_ClearValues[attachmentIndex].depthStencil = { 1.0f, 0 };
			}
			// Color attachment
			else
			{
                m_AttachmentImages.emplace_back(static_cast<VulkanImage2D*>(attachment.AttachImage));
                m_AttachmentFormats.push_back(imageFormat);
				const auto& clearColor = m_Specification.ClearColor;
				m_ClearValues[attachmentIndex].color = { {clearColor.r, clearColor.g, clearColor.b, clearColor.a} };
			}
			attachmentIndex++;
		}
		
        m_RenderPass = VulkanRenderPass::Create(device, m_AttachmentFormats, m_DepthAttachmentFormat, m_Specification.ClearOnLoad);
        
		// Create Framebuffer
		std::vector<VkImageView> attachments(m_AttachmentImages.size());
		for (uint32_t i = 0; i < m_AttachmentImages.size(); i++)
		{
			VulkanImage2D* image = m_AttachmentImages[i];
			
            attachments[i] = image->GetImageInfo().ImageView;
            ETH_CORE_TRACE("iamge info: image {0}", (const void*)(image->GetImageInfo().Image));
            ETH_CORE_TRACE("iamge info: image view {0}", (const void*)(image->GetImageInfo().ImageView));
            ETH_CORE_ASSERT(attachments[i]);
		}

		if (m_DepthAttachmentImage)
		{
			VulkanImage2D* image = m_DepthAttachmentImage;
			attachments.emplace_back(image->GetImageInfo().ImageView);
			ETH_CORE_ASSERT(attachments.back());
		}

		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = m_RenderPass->GetHandle();
		framebufferCreateInfo.attachmentCount = uint32_t(attachments.size());
		framebufferCreateInfo.pAttachments = attachments.data();
		framebufferCreateInfo.width = m_Width;
		framebufferCreateInfo.height = m_Height;
		framebufferCreateInfo.layers = 1;

		ETH_CORE_TRACE("Framebuffer with {0} attachmment created", uint32_t(attachments.size()));
		VK_CHECK_RESULT(vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &m_Framebuffer));
	}

}
