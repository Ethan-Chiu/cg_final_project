#pragma once

#include "../Vulkan.h"

namespace Ethane {
	
	class VulkanShaderCompiler 
	{
	public:
		struct UniformBuffer
		{
			std::string Name;
			uint32_t Size = 0;
			VkShaderStageFlagBits ShaderStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
		};

		struct ImageSampler
		{
			std::string Name;
			uint32_t DescriptorSet = 0;
			uint32_t ArraySize = 0;
			VkShaderStageFlagBits ShaderStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
		};

		struct ShaderDescriptorSetData
		{
			std::unordered_map<uint32_t, UniformBuffer*> UniformBuffers;
			std::unordered_map<uint32_t, ImageSampler> ImageSamplers;

			operator bool() const { return !(UniformBuffers.empty() && ImageSamplers.empty()); }
		};

		struct CompileParam
		{
			const std::string& FilePath;
			const std::string& Source;
			bool ForceCompile = true;
		};

		struct CompileOutput
		{
			std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>>& ShaderBinary;
		};

		struct ReflectParam
		{
			std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>>& ShaderBinary;
		};

		struct ReflectOutput
		{
			std::vector<ShaderDescriptorSetData>& ShaderDescriptorSets;
			std::vector<VkPushConstantRange>& PushConstantRanges;
		};

	private:
		struct CompileIntermediate
		{
			const std::unordered_map<VkShaderStageFlagBits, std::string>& ShaderSources;
		};

	public: 
		VulkanShaderCompiler(const VulkanShaderCompiler&) = delete;
		VulkanShaderCompiler& operator = (const VulkanShaderCompiler&) = delete;
		VulkanShaderCompiler();

		static VulkanShaderCompiler& GetInstance()
		{
			static VulkanShaderCompiler s;
			return s;
		}

		bool Compile(const CompileParam& params, CompileOutput& output);
		void Reflect(const ReflectParam& param, ReflectOutput& outputs);

		static const char* GetCacheDirectory();

	private:
		static void Init();

		std::unordered_map<VkShaderStageFlagBits, std::string> PreProcess(const std::string& source);
		void CompileOrGetVulkanBinaries(const CompileParam& params, const std::unordered_map<VkShaderStageFlagBits, std::string>& shaderSources,
			std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>>& outputBinary);
		void ReflectStage(VkShaderStageFlagBits stage, const std::vector<uint32_t>& shaderBinary,
			std::vector<ShaderDescriptorSetData>& shaderDescriptorSets,
			std::vector<VkPushConstantRange>& pushConstantRanges);
	};

}