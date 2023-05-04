//
//  VulkanUtils.h
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/4.
//

#pragma once

#include "VulkanContext.h"

namespace Ethane {
namespace Utils {

    static uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
        
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        
        ETH_CORE_ASSERT("Memory type not found");
        return 0;
    }
}}


/* VulkanUtils_h */
