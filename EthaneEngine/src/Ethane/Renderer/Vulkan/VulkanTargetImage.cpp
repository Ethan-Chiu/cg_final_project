//
//  VulkanRenderTargetImage.cpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/11.
//

#include "ethpch.h"

#include "VulkanTargetImage.h"
#include "VulkanSwapChain.h"
#include "VulkanUtils.h"

namespace Ethane {

    VulkanTargetImage::VulkanTargetImage(const VulkanContext* context, ImageSpecification specification)
        : m_Context(context), m_Specification(specification)
    {
        CreateTargetImages();
    }

    VulkanTargetImage::VulkanTargetImage(ImageSpecification specification, const std::vector<ImageUtils::VulkanImageInfo>& infos)
        :m_Specification(specification), m_Infos(std::move(infos))
    {
        m_SwapchainTarget = true;
    }

    VulkanTargetImage::VulkanTargetImage(const VulkanContext* context, uint32_t width, uint32_t height, uint32_t mip, uint32_t layers, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlag, VkImageAspectFlags aspectFlag, bool createView)
        :m_Context(context)
    {
        auto device = m_Context->GetDevice();
        
        uint32_t imageCount = m_Context->GetSwapchain()->GetImageCount();
        m_Infos.resize(imageCount);
        
        for(uint32_t i = 0; i <= imageCount; i++)
        {
            ImageUtils::CreateVulkanImage(device, width, height, mip, layers, format, tiling, usage, memoryFlag, m_Infos[i], m_ImageMemory[i]);
        }
        if (createView) {
            m_ImageMemory.resize(imageCount);
            for(uint32_t i = 0; i <= imageCount; i++)
            {
                ImageUtils::CreateImageView(device->GetVulkanDevice(), format, aspectFlag, mip, layers, m_Infos[i]);
            }
        }
    }

    VulkanTargetImage::~VulkanTargetImage()
    {
        Destroy();
    }

    void VulkanTargetImage::Destroy()
    {
        if (m_Infos.empty() || m_SwapchainTarget)
            return;

        auto device = m_Context->GetDevice()->GetVulkanDevice();
        vkDeviceWaitIdle(device);
        
        for(auto& info : m_Infos)
        {
            if (info.Image == nullptr)
                continue;
            
            if (info.ImageView) {
                vkDestroyImageView(device, info.ImageView, nullptr);
                ETH_CORE_WARN("VulkanImage2D::Release ImageView = {0}", (const void*)info.ImageView);
            }

            if (info.Image)
                vkDestroyImage(device, info.Image, nullptr);
            
            info.Image = nullptr;
            info.ImageView = nullptr;
        }
        
        for(auto& mem : m_ImageMemory)
        {
            if (mem) {
                vkFreeMemory(device, mem, nullptr);
                mem = nullptr;
            }
        }
    }

    void VulkanTargetImage::Resize(uint32_t width, uint32_t height)
    {
        if(m_SwapchainTarget)
            return;
        Destroy();
        m_Specification.Width = width;
        m_Specification.Height = height;
        CreateTargetImages();
    }

    void VulkanTargetImage::CreateTargetImages()
    {
        ETH_CORE_ASSERT(m_Specification.Width > 0 && m_Specification.Height > 0);
        ETH_CORE_ASSERT(m_Specification.Usage == ImageUsage::Attachment, "usage should be attachment in render target");
        
        ETH_CORE_TRACE("VulkanImage2D::Invalidate ({0})", m_Specification.DebugName);
        
        auto device = m_Context->GetDevice();
        
        VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
        if (Utils::IsDepthFormat(m_Specification.Format))
            usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        else
            usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        
        VkFormat vulkanFormat = ImageUtils::VulkanImageFormat(m_Specification.Format);
        
        VkImageAspectFlags aspectMask = Utils::IsDepthFormat(m_Specification.Format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
        if (m_Specification.Format == ImageFormat::DEPTH24STENCIL8)
            aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        
        uint32_t imageCount = m_Context->GetSwapchain()->GetImageCount();
        m_Infos.resize(imageCount);
        m_ImageMemory.resize(imageCount);
        
        // Create Image
        for(uint32_t i = 0; i <= imageCount; i++)
        {
            ImageUtils::CreateVulkanImage(device,
                                          m_Specification.Width,
                                          m_Specification.Height,
                                          m_Specification.Mips,
                                          m_Specification.Layers,
                                          vulkanFormat,
                                          VK_IMAGE_TILING_OPTIMAL,
                                          usage,
                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                          m_Infos[i],
                                          m_ImageMemory[i]);
            // Create Image view
            ImageUtils::CreateImageView(device->GetVulkanDevice(), vulkanFormat, aspectMask, m_Specification.Mips, m_Specification.Layers, m_Infos[i]);
        }
    }

    void VulkanTargetImage::SwapchainUpdate(ImageSpecification specification, const std::vector<ImageUtils::VulkanImageInfo>& infos)
    {
        m_Specification = specification;
        m_Infos = std::move(infos);
    }
}
