#pragma once
#include "VulkanShaderCompiler.h"
//#include "../VulkanUniformBufferSet.h"
#include "../VulkanShader.h"
//#include "../VulkanTexture.h"

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
//		static Ref<VulkanTexture2D> GetDefaultTexture() {
//			return s_DefaultTexture;
//		}

//		static void SetUniformBuffer(uint32_t set, uint32_t binding, const void* data, uint32_t size, uint32_t offset);

	protected:
		static bool ReflectBufferData(uint32_t set, uint32_t binding, VulkanShaderCompiler::UniformBuffer* uniform);
		static bool ReflectSamplerData(uint32_t set, uint32_t binding, VulkanShaderCompiler::ImageSampler sampler);

	private:
		// shader id

		// resource 
//		inline static Ref<VulkanUniformBufferSet> s_UniformBufferSet;
//		inline static Ref<VulkanTexture2D> s_DefaultTexture;

		// reflect data
		inline static std::vector<VulkanShaderCompiler::ShaderDescriptorSetData> s_ShaderDescriptorSets;
		inline static std::vector<VkPushConstantRange> s_PushConstantRanges;

	friend VulkanShaderCompiler;
	};

}
