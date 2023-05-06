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
		VulkanShader(const VulkanContext* ctx, const std::string& filepath);
		VulkanShader(const VulkanContext* ctx, const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) {}; // temp
		virtual ~VulkanShader();

		void Destroy() override;

		// Getter
		virtual const std::string& GetName() const override { return  m_Name; }
		const std::vector<VkPipelineShaderStageCreateInfo>& GetPipelineShaderStageCreateInfos() const { return m_PipelineShaderStageCreateInfos; }
		const std::vector<VkPushConstantRange> GetPushConstantRanges() const { return m_PushConstantRanges; } // TODO
		std::vector<VkDescriptorSetLayout> GetAllDescriptorSetLayouts();
		const std::vector<VulkanShaderCompiler::ShaderDescriptorSetData>& GetShaderDescriptorSetData() const { return m_ShaderDescriptorSetsReflect; }

		// TODO: remove this
		//Uniform
		virtual uint32_t GetUniformBufferIndex(uint32_t bindingPoint) { return 0; };
		virtual void SetUniformBuffer(uint32_t uboIndex, const void* data, uint32_t size, uint32_t offset = 0){};
		virtual void SetUniformBufferByBindingPoint(uint32_t bindingPoint, const void* data, uint32_t size, uint32_t offset = 0) {};
		virtual uint32_t GetUniformBufferIndex(const std::string& name) { return 0; }; // test
		virtual void SetUniformBufferByName(const std::string& name, const void* data, uint32_t size) {}; // test

	private:
		void CreatePipelineShaderStage(const std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>>& shaderData);
		void CreateDescriptorLayouts();

	public:
//		VkDescriptorPool CreateDescriptorPool(uint32_t numberOfSets = 1);
//		VkDescriptorSet CreateDescriptorSet(uint32_t set, VkDescriptorPool pool);
//		DescriptorSetsAndPool CreateDescriptorSetsAndPool(uint32_t set, uint32_t numberOfSets = 1);
//		DescriptorSetsAndPool CreateDescriptorSets(uint32_t set);
	
	private:
		std::string m_FilePath;
		std::string m_Name;
        
        const VulkanDevice* m_Device = nullptr;

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
		std::vector<std::unordered_map<std::string, WriteDescriptorSetBase>> m_WriteDescriptorSetsBase;
	};
}
