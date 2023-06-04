#include "ethpch.h"

#include "VulkanShader.h"

#include "Ethane/Core/timer.h"
#include "Ethane/Utils/FileUtils.hpp"
#include "ShaderUtils/VulkanShaderSystem.h"

namespace Ethane {

	VulkanShader::VulkanShader(const std::string& filepath)
		:m_FilePath(filepath)
	{
		ETH_PROFILE_FUNCTION();
		ETH_CORE_INFO("Create Shader: {0}", filepath);

		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);

		std::string source = FileUtils::ReadFile(filepath);

		Timer timer;

		VulkanShaderCompiler& compiler = VulkanShaderCompiler::GetInstance();
        std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>> shaderBinary;
		VulkanShaderCompiler::CompileOutput outputs{ shaderBinary };
		VulkanShaderCompiler::CompileParam param{ m_FilePath, source };
        param.ForceCompile = true;
		compiler.Compile(param, outputs);
		VulkanShaderCompiler::ReflectOutput reflectOutputs{ m_ShaderDescriptorSetsReflect, m_PushConstantRanges };
		VulkanShaderCompiler::ReflectParam reflectParam{ outputs.ShaderBinary };
		compiler.Reflect(reflectParam, reflectOutputs);
		
		// profiling
		ETH_CORE_TRACE("[Time] compile: {0}ms", timer.ElapsedMillis());
		timer.Reset();
        
        CreatePipelineShaderStage(shaderBinary);
        CreateDescriptorLayouts();
	}

	VulkanShader::~VulkanShader()
	{
	}

	void VulkanShader::Destroy()
	{
        VkDevice device = VulkanContext::GetDevice()->GetVulkanDevice();
        vkDeviceWaitIdle(device);
        
		for (auto descriptorSetLayout : m_DescriptorSetLayouts)
		{
			vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
		}

		for (auto& [flag, shadermodule] : m_ShaderModule)
		{
			// ETH_CORE_INFO("123 {0}", Utils::GLShaderStageToString(flag));
			vkDestroyShaderModule(device, shadermodule, nullptr);
		}
	}

	void VulkanShader::CreatePipelineShaderStage(const std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>>& shaderData)
	{
		VkDevice device = VulkanContext::GetDevice()->GetVulkanDevice();
		m_PipelineShaderStageCreateInfos.clear();
		for (auto [stage, data] : shaderData)
		{
			ETH_CORE_ASSERT(data.size());
			VkShaderModuleCreateInfo moduleCreateInfo{};

			moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			moduleCreateInfo.codeSize = data.size() * sizeof(uint32_t);
			moduleCreateInfo.pCode = data.data();

			VK_CHECK_RESULT(vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &m_ShaderModule[stage]));

			VkPipelineShaderStageCreateInfo& shaderStage = m_PipelineShaderStageCreateInfos.emplace_back();
			shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStage.stage = stage;
			shaderStage.module = m_ShaderModule[stage];
			shaderStage.pName = "main";
		}
	}

	void VulkanShader::CreateDescriptorLayouts()
	{
		VkDevice device = VulkanContext::GetDevice()->GetVulkanDevice();

		uint32_t setCount = (uint32_t)m_ShaderDescriptorSetsReflect.size();
		m_WriteDescriptorSetsBase.resize(setCount);
		for (uint32_t set = 0; set < setCount; set++)
		{
			auto& shaderDescriptorSet = m_ShaderDescriptorSetsReflect[set];

			// Uniform Buffers
			std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
			for (auto& [binding, uniformBuffer] : shaderDescriptorSet.UniformBuffers)
			{
				VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.emplace_back();
				layoutBinding.binding = binding;
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				layoutBinding.descriptorCount = 1;
				layoutBinding.stageFlags = uniformBuffer.ShaderStage;
				layoutBinding.pImmutableSamplers = nullptr;

				VkWriteDescriptorSet& wds = m_WriteDescriptorSetsBase[set][uniformBuffer.Name].WriteDescriptor;
				wds = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
				wds.descriptorType = layoutBinding.descriptorType;
				wds.dstBinding = layoutBinding.binding;
				wds.descriptorCount = 1;
			}

			// Image Sampler
			for (auto& [binding, imageSampler] : shaderDescriptorSet.ImageSamplers)
			{
				auto& layoutBinding = layoutBindings.emplace_back();
				layoutBinding.binding = binding;
				layoutBinding.descriptorCount = imageSampler.ArraySize;
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				layoutBinding.pImmutableSamplers = nullptr;
				layoutBinding.stageFlags = imageSampler.ShaderStage;

//				ETH_CORE_ASSERT(shaderDescriptorSet.UniformBuffers.find(binding) == shaderDescriptorSet.UniformBuffers.end(), "Binding is already present!");

				VkWriteDescriptorSet& wds = m_WriteDescriptorSetsBase[set][imageSampler.Name].WriteDescriptor;
				wds = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
				wds.descriptorType = layoutBinding.descriptorType;
				wds.dstBinding = layoutBinding.binding;
				wds.descriptorCount = imageSampler.ArraySize;
			}
            
            // Storage Buffers
            for (auto& [binding, storageBuffer] : shaderDescriptorSet.StorageBuffers)
            {
                VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.emplace_back();
                layoutBinding.binding = binding;
                layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                layoutBinding.descriptorCount = 1;
                layoutBinding.stageFlags = storageBuffer.ShaderStage;
                layoutBinding.pImmutableSamplers = nullptr;

                VkWriteDescriptorSet& wds = m_WriteDescriptorSetsBase[set][storageBuffer.Name].WriteDescriptor;
                wds = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
                wds.descriptorType = layoutBinding.descriptorType;
                wds.dstBinding = layoutBinding.binding;
                wds.descriptorCount = 1;
            }

            // Storage Image
            for (auto& [binding, imageSampler] : shaderDescriptorSet.StorageImage)
            {
                auto& layoutBinding = layoutBindings.emplace_back();
                layoutBinding.binding = binding;
                layoutBinding.descriptorCount = imageSampler.ArraySize;
                layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                layoutBinding.pImmutableSamplers = nullptr;
                layoutBinding.stageFlags = imageSampler.ShaderStage;

                VkWriteDescriptorSet& wds = m_WriteDescriptorSetsBase[set][imageSampler.Name].WriteDescriptor;
                wds = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
                wds.descriptorType = layoutBinding.descriptorType;
                wds.dstBinding = layoutBinding.binding;
                wds.descriptorCount = imageSampler.ArraySize;
            }

			VkDescriptorSetLayoutCreateInfo descriptorLayout = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
			descriptorLayout.pNext = nullptr;
			descriptorLayout.bindingCount = static_cast<uint32_t>(layoutBindings.size());
			descriptorLayout.pBindings = layoutBindings.data();

			ETH_CORE_INFO("Creating descriptor set {0} with {1} ubo's, {2} ssbo's, {3} samplers and {4} storage images", set,
				shaderDescriptorSet.UniformBuffers.size(), shaderDescriptorSet.StorageBuffers.size(), shaderDescriptorSet.ImageSamplers.size(), shaderDescriptorSet.StorageImage.size());

			if (set >= m_DescriptorSetLayouts.size())
				m_DescriptorSetLayouts.resize((size_t)(set + 1));
			VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &descriptorLayout, nullptr, &m_DescriptorSetLayouts[set]));
		}
	}

	// For pipeline creation
	std::vector<VkDescriptorSetLayout> VulkanShader::GetAllDescriptorSetLayouts()
	{
		std::vector<VkDescriptorSetLayout> result;
		result.reserve(m_DescriptorSetLayouts.size());
		for (auto& layout : m_DescriptorSetLayouts)
			result.emplace_back(layout);

		return result;
	}

	const VulkanShader::WriteDescriptorSetBase* VulkanShader::RetrieveWriteDescriptorSetBase(uint32_t set, const std::string& name) const
	{
		ETH_CORE_ASSERT(set < m_WriteDescriptorSetsBase.size());
		if (m_WriteDescriptorSetsBase[set].find(name) == m_WriteDescriptorSetsBase[set].end())
		{
			ETH_CORE_WARN("Shader {0} does not contain requested descriptor set {1}", m_Name, name);
			return nullptr;
		}
		return &m_WriteDescriptorSetsBase[set].at(name);
	}

	VkDescriptorPool VulkanShader::CreateDescriptorPool(uint32_t numberOfSets) const
	{
		VkDescriptorPool out_pool;
		std::vector<VkDescriptorPoolSize> poolSizes;
		uint64_t setCount = m_ShaderDescriptorSetsReflect.size();
		for (uint32_t set = 0; set < setCount; set++)
		{
			auto& shaderDescriptorSet = m_ShaderDescriptorSetsReflect[set];
			if (!shaderDescriptorSet) // Empty descriptor set
				continue;

			if (shaderDescriptorSet.UniformBuffers.size())
			{
				VkDescriptorPoolSize& typeCount = poolSizes.emplace_back();
				typeCount.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				typeCount.descriptorCount = (uint32_t)shaderDescriptorSet.UniformBuffers.size() * numberOfSets;
			}

			if (shaderDescriptorSet.ImageSamplers.size())
			{
				VkDescriptorPoolSize& typeCount = poolSizes.emplace_back();
				uint32_t descriptorSetCount = 0;
				for (auto&& [binding, imageSampler] : shaderDescriptorSet.ImageSamplers)
					descriptorSetCount += imageSampler.ArraySize;

				typeCount.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				typeCount.descriptorCount = descriptorSetCount * numberOfSets;
			}
            
            if (shaderDescriptorSet.StorageBuffers.size())
            {
                VkDescriptorPoolSize& typeCount = poolSizes.emplace_back();
                typeCount.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                typeCount.descriptorCount = (uint32_t)shaderDescriptorSet.StorageBuffers.size() * numberOfSets;
            }

            if (shaderDescriptorSet.StorageImage.size())
            {
                VkDescriptorPoolSize& typeCount = poolSizes.emplace_back();
                uint32_t descriptorSetCount = 0;
                for (auto&& [binding, imageSampler] : shaderDescriptorSet.StorageImage)
                    descriptorSetCount += imageSampler.ArraySize;

                typeCount.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                typeCount.descriptorCount = descriptorSetCount * numberOfSets;
            }
		}

		VkDescriptorPoolCreateInfo descriptorPoolInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
		descriptorPoolInfo.pNext = nullptr;
		descriptorPoolInfo.poolSizeCount = (uint32_t)poolSizes.size();
		descriptorPoolInfo.pPoolSizes = poolSizes.data();
		descriptorPoolInfo.maxSets = (uint32_t)(numberOfSets * setCount);

		VkDevice device = VulkanContext::GetDevice()->GetVulkanDevice();
		VK_CHECK_RESULT(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &out_pool));

		return out_pool;
	}

	VkDescriptorSet VulkanShader::CreateDescriptorSet(uint32_t set, VkDescriptorPool pool) const
	{
		VkDescriptorSet out_descriptorSet;
		VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocInfo.descriptorPool = pool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &m_DescriptorSetLayouts[set];

		VkDevice device = VulkanContext::GetDevice()->GetVulkanDevice();
		VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, &out_descriptorSet));
		return out_descriptorSet;
	}


}
