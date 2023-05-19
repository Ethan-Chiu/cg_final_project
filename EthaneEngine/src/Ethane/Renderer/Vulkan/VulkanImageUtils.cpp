//
//  VulkanImageUtils.cpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/16.
//

#include "VulkanImageUtils.h"
#include "VulkanUtils.h"

namespace Ethane {
namespace ImageUtils {

void CreateVulkanImage(const VulkanDevice* device, uint32_t width, uint32_t height, uint32_t mip, uint32_t layers, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlag, VulkanImageInfo& outInfo, VkDeviceMemory& memory)
{
    VkDevice vk_device = device->GetVulkanDevice();
    
    // Create Image
    VkImageCreateInfo imageInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mip;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.flags = 0; // Optional
    VK_CHECK_RESULT(vkCreateImage(vk_device, &imageInfo, nullptr, &outInfo.Image));

    //Allocate image memory
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(vk_device, outInfo.Image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = Utils::FindMemoryType(device->GetPhysicalDevice()->GetVulkanPhysicalDevice(), memRequirements.memoryTypeBits, memoryFlag);
    VK_CHECK_RESULT(vkAllocateMemory(vk_device, &allocInfo, nullptr, &memory));

    // Bind the memory
    VK_CHECK_RESULT(vkBindImageMemory(vk_device, outInfo.Image, memory, 0));
}

void CreateImageView(VkDevice device, VkFormat format, VkImageAspectFlags aspectMask, uint32_t mips, uint32_t layers, VulkanImageInfo& outInfo)
{
    VkImageViewCreateInfo viewInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    viewInfo.image = outInfo.Image;
    viewInfo.viewType = layers > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectMask;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mips;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = layers;
    viewInfo.flags = 0;
    VK_CHECK_RESULT(vkCreateImageView(device, &viewInfo, nullptr, &outInfo.ImageView));
}

}}
