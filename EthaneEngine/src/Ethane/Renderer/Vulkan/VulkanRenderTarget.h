//
//  VulkanRenderTarget.hpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/20.
//

#pragma once

#include "Ethane/Renderer/RenderTarget.h"
#include "VulkanContext.h"
#include "VulkanImage.h"
#include "VulkanTargetImage.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"

namespace Ethane {
class VulkanRenderTarget : public RenderTarget
{
public:
    VulkanRenderTarget(const RenderTargetSpecification& spec);

    virtual void Resize(uint32_t width, uint32_t height) override;
    virtual void Destroy() override;
    
    // Getter
    virtual const RenderTargetSpecification& GetSpecification() const override { return m_Specification; }
    virtual const VulkanRenderPass* GetRenderPass() const override { return m_RenderPass.get(); }
    const VulkanFramebuffer* GetCurrentFramebuffer() const;
    
    uint32_t GetWidth() const { return m_Width;}
    uint32_t GetHeight() const { return m_Height;}
    size_t GetColorAttachmentCount() const { return m_AttachmentImages.size(); }
    bool HasDepthAttachment() const { return (bool)m_DepthAttachmentImage; }
    const std::vector<VkClearValue>& GetVulkanClearValues() const { return m_ClearValues; }
    
private:
    void Invalidate();
    
private:
    RenderTargetSpecification m_Specification;
    uint32_t m_Width = 0, m_Height = 0;
    
    std::vector<Image*> m_AttachmentImages;
    VulkanImage2D* m_DepthAttachmentImage = nullptr;
    std::vector<std::vector<VkImageView>> m_ImageViewsSets;
    
    std::vector<ImageFormat> m_AttachmentFormats;
    ImageFormat m_DepthAttachmentFormat = ImageFormat::None;
    
    std::vector<VkClearValue> m_ClearValues;
    
    Scope<VulkanRenderPass> m_RenderPass;
    std::vector<Scope<VulkanFramebuffer>> m_Framebuffers;
};
}
