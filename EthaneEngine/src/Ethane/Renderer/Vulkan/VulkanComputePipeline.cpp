//
//  VulkanComputePipeline.cpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/25.
//

#include "ethpch.h"
#include "VulkanComputePipeline.h"
#include "VulkanContext.h"
#include "VulkanShader.h"

namespace Ethane {

    VulkanComputePipeline::VulkanComputePipeline(const ComputePipelineSpecification& spec)
        :m_Specification(spec), m_Device(VulkanContext::GetDevice())
    {
        Create();
    }

    VulkanComputePipeline::~VulkanComputePipeline()
    {
    }

    void VulkanComputePipeline::Destroy()
    {
        VkDevice device = m_Device->GetVulkanDevice();
        vkDeviceWaitIdle(device);
        vkDestroyPipeline(device, m_ComputePipeline, nullptr);
        vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
    }

    void VulkanComputePipeline::Create()
{
        // Shader Stage
        Ref<VulkanShader> vulkanShader = std::dynamic_pointer_cast<VulkanShader>(m_Specification.Shader);
        VkDevice device = m_Device->GetVulkanDevice();
        
        const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages = vulkanShader->GetPipelineShaderStageCreateInfos();
        
        auto descriptorSetLayouts = vulkanShader->GetAllDescriptorSetLayouts();
        auto vulkanPushConstantRanges = vulkanShader->GetPushConstantRanges();
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = (uint32_t)descriptorSetLayouts.size();
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = (uint32_t)vulkanPushConstantRanges.size();
        pipelineLayoutInfo.pPushConstantRanges = vulkanPushConstantRanges.data();
        VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout));
        
        VkComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.layout = m_PipelineLayout;
        pipelineInfo.stage = shaderStages[0];
        
        if (vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_ComputePipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create compute pipeline!");
        }
    }

    void VulkanComputePipeline::Bind(const VulkanCommandBuffer* cmdBuffer, VkPipelineBindPoint bindPoint)
    {
        vkCmdBindPipeline(cmdBuffer->GetHandle(), bindPoint, m_ComputePipeline);
    }
}
