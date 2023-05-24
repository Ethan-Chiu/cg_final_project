//
//  VulkanRenderTargetImage.hpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/11.
//

#pragma once

#include "Ethane/Renderer/Image.h"
#include "VulkanContext.h"
#include "VulkanImage.h"

namespace Ethane {
    class VulkanTargetImage: public TargetImage
    {
    public:
        VulkanTargetImage(ImageSpecification specification);
        VulkanTargetImage(ImageSpecification specification, const std::vector<ImageUtils::VulkanImageInfo>& infos);
        
        virtual ~VulkanTargetImage() override;

        VulkanTargetImage(const VulkanContext* context, uint32_t width, uint32_t height, uint32_t mip, uint32_t layers, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlag, VkImageAspectFlags aspectFlag, bool createView);

        void Resize(uint32_t width, uint32_t height);
        virtual void Destroy() override;

        // Getter
        virtual uint32_t GetWidth() const override { return m_Specification.Width; }
        virtual uint32_t GetHeight() const override { return m_Specification.Height; }
        virtual float GetAspectRatio() const override { return (float)m_Specification.Width / (float)m_Specification.Height; }
        virtual ImageSpecification& GetSpecification() override { return m_Specification; }
        virtual const ImageSpecification& GetSpecification() const override { return m_Specification; }

        ImageUtils::VulkanImageInfo& GetImageInfo(uint32_t idx) { return m_Infos[idx]; }
        const ImageUtils::VulkanImageInfo& GetImageInfo(uint32_t idx) const { return m_Infos[idx]; }
            
    protected:
        void SwapchainUpdate(ImageSpecification specification, const std::vector<ImageUtils::VulkanImageInfo>& infos);
        
    private:
        void CreateTargetImages();

    private:
        ImageSpecification m_Specification;
        bool m_SwapchainTarget = false;
        std::vector<ImageUtils::VulkanImageInfo> m_Infos;
        std::vector<VkDeviceMemory> m_ImageMemory;
    
    friend class VulkanSwapChain;
    };
}
