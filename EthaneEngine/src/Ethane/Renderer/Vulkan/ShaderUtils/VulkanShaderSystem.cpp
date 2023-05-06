#include "ethpch.h"

#include "VulkanShaderSystem.h"

#include "../VulkanContext.h"
#include "VulkanShaderCompiler.h"
//#include "../VulkanRendererAPI.h"

namespace Ethane {

	bool VulkanShaderSystem::Init() 
	{
		ETH_CORE_INFO("Vulkan Shader System Init");
//		s_UniformBufferSet = CreateRef<VulkanUniformBufferSet>(3);
//		s_DefaultTexture = CreateRef<VulkanTexture2D>("assets/textures/test.png");
//
//		for (uint32_t set = 0; set < s_ShaderDescriptorSets.size(); ++set) {
//			for (auto&& [binding, pShaderUBO] : s_ShaderDescriptorSets[set].UniformBuffers) {
//				s_UniformBufferSet->Create(pShaderUBO->Size, set, binding);
//			}
//		}

		return true;
	};

	void VulkanShaderSystem::Shutdown() 
	{
		ETH_CORE_INFO("Vulkan Shader System Shutdown");
	};

	bool VulkanShaderSystem::ReflectBufferData(uint32_t set, uint32_t binding, VulkanShaderCompiler::UniformBuffer* uniform)
	{
		if (set >= s_ShaderDescriptorSets.size()) {
			s_ShaderDescriptorSets.resize(set+1);
		}
		s_ShaderDescriptorSets[set].UniformBuffers[binding] = uniform;
		return true;
	}

	bool VulkanShaderSystem::ReflectSamplerData(uint32_t set, uint32_t binding, VulkanShaderCompiler::ImageSampler sampler)
	{
		if (set == 0) {
			s_ShaderDescriptorSets[set].ImageSamplers[binding] = sampler;
		}
		return true;
	}

//	void VulkanShaderSystem::SetUniformBuffer(uint32_t set, uint32_t binding, const void* data, uint32_t size, uint32_t offset)
//	{
//		uint32_t bufferIndex = VulkanContext::GetSwapChain()->GetCurrentFrameIndex();
//		s_UniformBufferSet->Get(bufferIndex, set, binding)->SetData(data, size, offset);
//	}
}
