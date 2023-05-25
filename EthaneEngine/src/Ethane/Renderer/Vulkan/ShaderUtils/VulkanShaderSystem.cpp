#include "ethpch.h"

#include "VulkanShaderSystem.h"

#include "../VulkanContext.h"
#include "VulkanShaderCompiler.h"


namespace Ethane {

	bool VulkanShaderSystem::Init() 
	{
		ETH_CORE_INFO("Vulkan Shader System Init");

		return true;
	};

	void VulkanShaderSystem::Shutdown() 
	{
		ETH_CORE_INFO("Vulkan Shader System Shutdown");
        
        // release global buffer
        for(auto&& [binding, uniformBuf]: s_GlobalUniformBuffers)
        {
            uniformBuf->Destroy();
        }
        
        // release shader buffer
        for(auto&& [shader, uniformSets]: s_ShaderUniformBuffers)
        {
            for (auto& uniformSet: uniformSets)
            {
                for(auto&& [binding, uniformBuf]: uniformSet)
                {
                    uniformBuf->Destroy();
                }
            }
        }
        
        // release shader buffer
//        for(auto&& [shader, storageSets]: s_ShaderStorageBuffers)
//        {
//            for (auto& storageSet: storageSets)
//            {
//                for(auto&& [binding, storageBuf]: storageSet)
//                {
//                    storageBuf->Destroy();
//                }
//            }
//        }
	};

    void VulkanShaderSystem::RegisterShader(const VulkanShader* shader)
    {
        auto& resourceData = shader->GetShaderDescriptorSetData();
        uint32_t setCount = (uint32_t)resourceData.size();
        
        if (setCount > s_ShaderDescriptorSets[shader].size()) {
            s_ShaderDescriptorSets[shader].resize(setCount);
        }
        
        // Global set
        if (setCount >= 1)
        {
            for(auto&& [binding, pShaderUBO] : resourceData[0].UniformBuffers) {
                uint32_t size = pShaderUBO.Size;
                if (s_GlobalUniformBuffers.find(binding) == s_GlobalUniformBuffers.end())
                {
                    Ref<VulkanUniformBuffer> uniformBuffer = CreateRef<VulkanUniformBuffer>(size, binding);
                    s_GlobalUniformBuffers[binding] = uniformBuffer;
                    s_GlobalDescriptorSets.UniformBuffers[binding] = pShaderUBO;
                }
                else
                {
                    if (size > s_GlobalUniformBuffers[binding]->GetSize())
                    {
                        s_GlobalUniformBuffers[binding]->Resize(size);
                        s_GlobalDescriptorSets.UniformBuffers[binding] = pShaderUBO;
                    }
                }
            }
        }
        
        // Instance set
        auto& shaderUniformBuffers = s_ShaderUniformBuffers[shader];
        for (uint32_t set = 1; set < setCount; set++)
        {
            for(auto&& [binding, pShaderUBO] : resourceData[set].UniformBuffers) {
                uint32_t size = pShaderUBO.Size * 128;
                if (shaderUniformBuffers[set].find(binding) == shaderUniformBuffers.at(set).end())
                {
                    Ref<VulkanUniformBuffer> uniformBuffer = CreateRef<VulkanUniformBuffer>(size, binding);
                }
                else
                {
                    if (size > shaderUniformBuffers[0][binding]->GetSize())
                        shaderUniformBuffers[0][binding]->Resize(size);
                }
            }
            s_ShaderDescriptorSets[shader][set] = resourceData[set];
        }
        
        s_CurrentInstanceCount[shader] = 0;
    }

    uint32_t VulkanShaderSystem::RegisterShaderInstance(const VulkanShader* shader)
    {
        return s_CurrentInstanceCount[shader]++;
    }
    
    VkDescriptorBufferInfo VulkanShaderSystem::GetUniformBufferInfo(uint32_t set, uint32_t binding, const VulkanMaterial* material)
    {
        VkDescriptorBufferInfo bufferInfo;
        if (set == 0)
        {
            bufferInfo.buffer = s_GlobalUniformBuffers[binding]->GetHandle();
            bufferInfo.offset = 0;
            bufferInfo.range = s_GlobalDescriptorSets.UniformBuffers[binding].Size;
            return bufferInfo;
        }
        uint32_t uboSize = s_ShaderDescriptorSets[static_cast<const VulkanShader*>(material->GetShader())][set].UniformBuffers[binding].Size;
        bufferInfo.buffer = s_ShaderUniformBuffers[static_cast<const VulkanShader*>(material->GetShader())][set][binding]->GetHandle();
        bufferInfo.offset = uboSize * (material->GetResourceId());
        bufferInfo.range = uboSize;
        return bufferInfo;
    }

	void VulkanShaderSystem::SetGlobalUniformBuffer(uint32_t binding, const void* data, uint32_t size)
	{
        s_GlobalUniformBuffers[binding]->SetData(data, 0, size, 0, 0);
	}

    void VulkanShaderSystem::SetInstanceUniformBuffer(uint32_t set, uint32_t binding, const VulkanMaterial* material, const void* data, uint32_t size)
    {
        s_ShaderUniformBuffers[static_cast<const VulkanShader*>(material->GetShader())][set][binding]->SetData(data, 0, size, 0, 0);
    }
}
