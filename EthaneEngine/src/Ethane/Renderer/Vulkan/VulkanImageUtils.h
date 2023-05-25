//
//  VulkanImageUtils.h
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/16.
//

#pragma once

#include "Vulkan.h"
#include "VulkanDevice.h"
#include "Ethane/Renderer/Image.h"

namespace Ethane {
namespace ImageUtils {

    struct VulkanImageInfo
    {
        VkImage Image = nullptr;
        VkImageView ImageView = nullptr;
    };


    void CreateVulkanImage(const VulkanDevice* device, uint32_t width, uint32_t height, uint32_t mip, uint32_t layers, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlag, VulkanImageInfo& outInfo, VkDeviceMemory& memory);

    void CreateImageView(VkDevice device, VkFormat format, VkImageAspectFlags aspectMask, uint32_t mips, uint32_t layers, VulkanImageInfo& outInfo);


    inline VkFormat VulkanImageFormat(ImageFormat format)
    {
        switch (format)
        {
        case ImageFormat::RED32F:          return VK_FORMAT_R32_SFLOAT;
        case ImageFormat::RG16F:           return VK_FORMAT_R16G16_SFLOAT;
        case ImageFormat::RG32F:           return VK_FORMAT_R32G32_SFLOAT;
        case ImageFormat::SRGB:            return VK_FORMAT_R8G8B8_SRGB;
        case ImageFormat::RGB:             return VK_FORMAT_R8G8B8_UNORM;
        case ImageFormat::BGR:             return VK_FORMAT_B8G8R8_UNORM;
        case ImageFormat::RGBA:            return VK_FORMAT_R8G8B8A8_UNORM;
        case ImageFormat::RGBA16F:         return VK_FORMAT_R16G16B16A16_SFLOAT;
        case ImageFormat::RGBA32F:         return VK_FORMAT_R32G32B32A32_SFLOAT;
        case ImageFormat::BGRA:            return VK_FORMAT_B8G8R8A8_UNORM;
        case ImageFormat::DEPTH32F:        return VK_FORMAT_D32_SFLOAT;
        case ImageFormat::DEPTH24STENCIL8: return VK_FORMAT_D24_UNORM_S8_UINT;// VulkanContext::GetDevice()->GetPhysicalDevice()->GetDepthFormat();
        }
        ETH_CORE_ASSERT(false);
        ETH_CORE_WARN("Image format is {0}", (uint8_t)format);
        return VK_FORMAT_UNDEFINED;
    }

    inline ImageFormat VulkanImageFormatToImageFormat(VkFormat format)
    {
        switch (format)
        {
            case VK_FORMAT_R32_SFLOAT:              return ImageFormat::RED32F;
            case VK_FORMAT_R16G16_SFLOAT:           return ImageFormat::RG16F;
            case VK_FORMAT_R32G32_SFLOAT:           return ImageFormat::RG32F;
            case VK_FORMAT_R8G8B8A8_UNORM:          return ImageFormat::RGBA;
            case VK_FORMAT_R16G16B16A16_SFLOAT:     return ImageFormat::RGBA16F;
            case VK_FORMAT_R32G32B32A32_SFLOAT:     return ImageFormat::RGBA32F;
            case VK_FORMAT_R8G8B8_SRGB:             return ImageFormat::SRGB;
            case VK_FORMAT_R8G8B8_UNORM:            return ImageFormat::RGB;
            case VK_FORMAT_B8G8R8_UNORM:            return ImageFormat::BGR;
            case VK_FORMAT_B8G8R8A8_UNORM:          return ImageFormat::BGRA;
            case VK_FORMAT_D32_SFLOAT:              return ImageFormat::DEPTH32F;
            case VK_FORMAT_D24_UNORM_S8_UINT:       return ImageFormat::DEPTH24STENCIL8;
        }
        ETH_CORE_ASSERT(false);
        ETH_CORE_WARN("Image format is {0}", format);
        return ImageFormat::None;
    }
}}
