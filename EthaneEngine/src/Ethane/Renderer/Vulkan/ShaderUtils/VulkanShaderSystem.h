#pragma once
#include "VulkanShaderCompiler.h"
//#include "../VulkanUniformBufferSet.h"
#include "../VulkanShader.h"
#include "../VulkanTexture.h"

#include "../VulkanUniformBuffer.h"
#include "../VulkanMaterial.h"

namespace Ethane {

	class VulkanShaderSystem {

	public:
		static bool Init();
		static void Shutdown();

	public:
//		static Ref<VulkanUniformBuffer> GetUniformBuffer(uint32_t frame, uint32_t set, uint32_t binding) {
//			return s_UniformBufferSet->Get(frame, set, binding);
//		}
//
        static void RegisterShader(const VulkanShader* shader);
        static uint32_t RegisterShaderInstance(const VulkanShader* shader);
        
        static VkDescriptorBufferInfo GetUniformBufferInfo(uint32_t set, uint32_t binding, const VulkanMaterial* material);
        
        static void SetGlobalUniformBuffer(uint32_t binding, const void* data, uint32_t size);
        static void SetInstanceUniformBuffer(uint32_t set, uint32_t binding, const VulkanMaterial* material, const void* data, uint32_t size);

	private:
		// resource 
        inline static std::map<uint32_t, Ref<VulkanUniformBuffer>> s_GlobalUniformBuffers; // binding
        inline static std::map<const VulkanShader* ,std::vector<std::map<uint32_t, Ref<VulkanUniformBuffer>>>> s_ShaderUniformBuffers; // shader->set->binding
        
//        inline static std::map<const VulkanShader* ,std::vector<std::map<uint32_t, Ref<VulkanUniformBuffer>>>> s_ShaderStorageBuffers; // shader->set->binding
        

        inline static std::map<const VulkanShader* ,uint32_t> s_CurrentInstanceCount;
        
		// reflect data
		inline static VulkanShaderCompiler::ShaderDescriptorSetData s_GlobalDescriptorSets;
        inline static std::map<const VulkanShader*, std::vector<VulkanShaderCompiler::ShaderDescriptorSetData>> s_ShaderDescriptorSets;

	friend VulkanShaderCompiler;
	};

}
