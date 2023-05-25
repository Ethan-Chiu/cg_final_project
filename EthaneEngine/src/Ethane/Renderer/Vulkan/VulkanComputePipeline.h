//
//  VulkanComputePipeline.hpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/25.
//

#pragma once
#include "Ethane/Renderer/Pipeline.h"
#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"

namespace Ethane {

class VulkanComputePipeline : public ComputePipeline
{    
public:
    VulkanComputePipeline() = default;
    VulkanComputePipeline(const ComputePipelineSpecification& spec);
    virtual ~VulkanComputePipeline() override;
    
    void Destroy() override;

    void Bind(const VulkanCommandBuffer* cmdBuffer, VkPipelineBindPoint bindPoint);

    // Getter
    VkPipeline GetVulkanPipeline() { return m_ComputePipeline; }
    VkPipelineLayout GetPipelineLayout() { return m_PipelineLayout; }
    virtual ComputePipelineSpecification& GetSpecification() override { return m_Specification; }
    virtual const ComputePipelineSpecification& GetSpecification() const override { return m_Specification; }
private:
    void Create();
    
private:
    ComputePipelineSpecification m_Specification;

    const VulkanDevice* m_Device = nullptr;
    VkPipeline m_ComputePipeline;
    VkPipelineLayout m_PipelineLayout;
};

}

