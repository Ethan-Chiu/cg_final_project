#pragma once

#include "Ethane/Renderer/Pipeline.h"

#include "VulkanContext.h"
#include "VulkanCommandBuffer.h"

namespace Ethane {

	class VulkanPipeline : public Pipeline
	{

	public:
		VulkanPipeline() = default;
		VulkanPipeline(const PipelineSpecification& spec);
		virtual ~VulkanPipeline() override;
		
		void Destroy() override;

		void Bind(VkCommandBuffer cmdBuffer, VkPipelineBindPoint bindPoint);

		// Getter
		VkPipeline GetVulkanPipeline() { return m_GraphicsPipeline; }
		VkPipelineLayout GetPipelineLayout() { return m_PipelineLayout; }
		virtual PipelineSpecification& GetSpecification() override { return m_Specification; }
		virtual const PipelineSpecification& GetSpecification() const override { return m_Specification; }
    private:
        void Create();
	private:
		PipelineSpecification m_Specification;

        const VulkanDevice* m_Device = nullptr;
		VkPipeline m_GraphicsPipeline;
		VkPipelineLayout m_PipelineLayout; // test 
	};

}
