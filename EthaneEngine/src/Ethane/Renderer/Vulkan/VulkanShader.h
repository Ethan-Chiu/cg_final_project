#pragma once

#include "Ethane/Renderer/Shader.h"

#include "VulkanContext.h"

#include "ShaderUtils/VulkanShaderCompiler.h"


namespace Ethane {

	class VulkanShader : public Shader
	{
	public:
		struct DescriptorSetsAndPool
		{
			VkDescriptorPool Pool = nullptr;
			std::vector<VkDescriptorSet> DescriptorSets;
		};

	public:
		VulkanShader() = default;
		VulkanShader(const std::string& filepath);
		VulkanShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) {}; // temp
		virtual ~VulkanShader();

		void Destroy() override;

		// Getter
		virtual const std::string& GetName() const override { return  m_Name; }
        
    friend class VulkanPipeline;
    friend class VulkanShaderSystem;
    protected:
		const std::vector<VkPipelineShaderStageCreateInfo>& GetPipelineShaderStageCreateInfos() const { return m_PipelineShaderStageCreateInfos; }
		const std::vector<VkPushConstantRange> GetPushConstantRanges() const { return m_PushConstantRanges; }
		std::vector<VkDescriptorSetLayout> GetAllDescriptorSetLayouts();
		const std::vector<VulkanShaderCompiler::ShaderDescriptorSetData>& GetShaderDescriptorSetData() const { return m_ShaderDescriptorSetsReflect; }
        
    public:
        VkDescriptorPool CreateDescriptorPool(uint32_t numberOfSets = 1) const;
        VkDescriptorSet CreateDescriptorSet(uint32_t set, VkDescriptorPool pool) const;
        

	private:
		void CreatePipelineShaderStage(const std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>>& shaderData);
		void CreateDescriptorLayouts();
	
	private:
		std::string m_FilePath;
		std::string m_Name;
        
		// Data from Reflect
		std::vector<VulkanShaderCompiler::ShaderDescriptorSetData> m_ShaderDescriptorSetsReflect;
		std::vector<VkPushConstantRange> m_PushConstantRanges;

		// Discripter Set Layouts
		std::vector<VkDescriptorSetLayout> m_DescriptorSetLayouts;

		// Descriptor pool size info
		std::unordered_map<uint32_t, std::vector<VkDescriptorPoolSize>> m_DescriptorCounts;

		// For pipeine creation
		std::unordered_map<VkShaderStageFlagBits, VkShaderModule>  m_ShaderModule;
		std::vector<VkPipelineShaderStageCreateInfo> m_PipelineShaderStageCreateInfos;

	public:
		struct WriteDescriptorSetBase {
			VkWriteDescriptorSet WriteDescriptor;
		};

		const WriteDescriptorSetBase* RetrieveWriteDescriptorSetBase(uint32_t set, const std::string& name) const;
		const std::vector<std::unordered_map<std::string, WriteDescriptorSetBase>>& RetrieveWriteDescriptorSetsBase() const {
			return m_WriteDescriptorSetsBase;
		};
	private:
		// Write Descriptors Templates
		std::vector<std::unordered_map<std::string, WriteDescriptorSetBase>> m_WriteDescriptorSetsBase; // set -> [name, write_descriptor_set]
	};
}
