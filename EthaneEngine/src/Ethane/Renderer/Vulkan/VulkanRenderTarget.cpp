//
//  VulkanRenderTarget.cpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/20.
//
#include "ethpch.h"
#include "VulkanRenderTarget.h"
#include "VulkanSwapChain.h"

namespace Ethane {

    VulkanRenderTarget::VulkanRenderTarget(const RenderTargetSpecification& spec)
        :m_Specification(spec)
    {
        auto device = VulkanContext::GetDevice()->GetVulkanDevice();
        m_Width = m_Specification.Width;
        m_Height = m_Specification.Height;
        
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
                m_AttachmentImages.emplace_back(attachment.AttachImage);
                m_AttachmentFormats.push_back(imageFormat);
                const auto& clearColor = m_Specification.ClearColor;
                m_ClearValues[attachmentIndex].color = { {clearColor.r, clearColor.g, clearColor.b, clearColor.a} };
            }
            attachmentIndex++;
        }
        
        m_RenderPass = VulkanRenderPass::Create(device, m_AttachmentFormats, m_DepthAttachmentFormat, m_Specification.ClearOnLoad, m_Specification.IsTargetImage);
        
        if (m_Specification.IsTargetImage)
        {
            uint32_t imageCount = VulkanContext::GetSwapchain()->GetImageCount();
            m_ImageViewsSets.resize(imageCount);
            m_Framebuffers.resize(imageCount);
        }
        else
        {
            m_ImageViewsSets.resize(1);
            m_Framebuffers.resize(1);
        }
        
        for(auto& imageViewsSet : m_ImageViewsSets)
        {
            imageViewsSet.resize(m_Specification.Attachments.Attachments.size());
        }
        for (auto& framebuffer : m_Framebuffers)
        {
            framebuffer = CreateScope<VulkanFramebuffer>(VulkanContext::GetDevice(), m_RenderPass.get());
        }
        
        Invalidate();
    }

    void VulkanRenderTarget::Resize(uint32_t width, uint32_t height)
    {
         if (m_Width == width && m_Height == height)
             return;

        m_Width = width;
        m_Height = height;
        
        if (m_Specification.IsTargetImage)
        {
            for (auto attachmentImage : m_AttachmentImages)
                static_cast<VulkanTargetImage*>(attachmentImage)->Resize(width, height);
        }
        else
        {
            for (auto attachmentImage : m_AttachmentImages)
                static_cast<VulkanImage2D*>(attachmentImage)->Resize(width, height);
        }
        
        if(m_DepthAttachmentImage)
        {
            m_DepthAttachmentImage->Resize(width, height);
        }
        
        Invalidate();
    }
    
    void VulkanRenderTarget::Invalidate()
    {
        ETH_CORE_TRACE("VulkanRenderTarget::Invalidate ({})", m_Specification.DebugName);
        
        if (m_Specification.IsTargetImage)
        {
            uint32_t imageCount = VulkanContext::GetSwapchain()->GetImageCount();
            for(uint32_t targetIdx = 0; targetIdx < m_AttachmentImages.size(); targetIdx++)
            {
                for (uint32_t i = 0; i < imageCount; i++)
                {
                    m_ImageViewsSets[i][targetIdx] = static_cast<VulkanTargetImage*>(m_AttachmentImages[targetIdx])->GetImageInfo(i).ImageView;
                }
            }
        }
        else
        {
            for(uint32_t targetIdx = 0; targetIdx < m_AttachmentImages.size(); targetIdx++)
            {
                m_ImageViewsSets[0][targetIdx] = static_cast<VulkanImage2D*>(m_AttachmentImages[targetIdx])->GetImageInfo().ImageView;
            }
        }
        
        // Has Depth
        if(m_DepthAttachmentImage)
        {
            for(auto& imageViewsSet : m_ImageViewsSets)
            {
                imageViewsSet[m_AttachmentImages.size()] = m_DepthAttachmentImage->GetImageInfo().ImageView;
            }
        }
        
        for(uint32_t set = 0; set < m_ImageViewsSets.size(); set++)
        {
            m_Framebuffers[set]->Invalidate(m_Width, m_Height, m_ImageViewsSets[set]);
        }
    }

    void VulkanRenderTarget::Destroy()
    {
        for (auto& framebuffer : m_Framebuffers)
        {
            framebuffer->Destroy();
        }
        
        m_RenderPass->Destroy();
    }

    const VulkanFramebuffer* VulkanRenderTarget::GetCurrentFramebuffer() const
    {
        if(m_Specification.IsTargetImage)
        {
            uint32_t imageIndex = VulkanContext::GetSwapchain()->GetCurrentImageIndex();
            return m_Framebuffers[imageIndex].get();
        }
        else
        {
            return m_Framebuffers[0].get();
        }
    }

}
