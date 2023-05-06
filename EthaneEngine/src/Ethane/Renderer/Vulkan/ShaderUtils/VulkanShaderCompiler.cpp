#include "ethpch.h"
#include "VulkanShaderCompiler.h"

#include "../VulkanContext.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include "Ethane/Core/timer.h"
#include "VulkanShaderSystem.h"

namespace Ethane {


	namespace Utils {

		static VkShaderStageFlagBits ShaderTypeFromString(const std::string& type)
		{
			if (type == "vertex")                       return VK_SHADER_STAGE_VERTEX_BIT;
			if (type == "fragment" || type == "pixel")  return VK_SHADER_STAGE_FRAGMENT_BIT;
			if (type == "compute")                      return VK_SHADER_STAGE_COMPUTE_BIT;

			return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
		}

		static shaderc_shader_kind VkShaderStageToShaderC(VkShaderStageFlagBits stage)
		{
			switch (stage)
			{
			case VK_SHADER_STAGE_VERTEX_BIT:    return shaderc_vertex_shader;
			case VK_SHADER_STAGE_FRAGMENT_BIT:  return shaderc_fragment_shader;
			case VK_SHADER_STAGE_COMPUTE_BIT:   return shaderc_compute_shader;
            default: break;
			}
			ETH_CORE_ASSERT(false);
			return (shaderc_shader_kind)0;
		}

		static const char* GLShaderStageToString(VkShaderStageFlagBits stage)
		{
			switch (stage)
			{
			case VK_SHADER_STAGE_VERTEX_BIT:   return "VERTEX_SHADER";
			case VK_SHADER_STAGE_FRAGMENT_BIT: return "FRAGMENT_SHADER";
			case VK_SHADER_STAGE_COMPUTE_BIT:  return "COMPUTE_SHADER";
            default: break;
			}
			ETH_CORE_ASSERT(false);
			return nullptr;
		}

		static const char* VkShaderStageCachedFileExtension(VkShaderStageFlagBits stage)
		{
			switch (stage)
			{
			case VK_SHADER_STAGE_VERTEX_BIT:    return ".cached_vulkan.vert";
			case VK_SHADER_STAGE_FRAGMENT_BIT:  return ".cached_vulkan.frag";
			case VK_SHADER_STAGE_COMPUTE_BIT:   return ".cached_vulkan.comp";
            default: break;
			}
			ETH_CORE_ASSERT(false);
			return "";
		}
	}

	VulkanShaderCompiler::VulkanShaderCompiler()
	{
		Init();
	}

	void VulkanShaderCompiler::Init() 
	{
		std::string cacheDirectory = GetCacheDirectory();
		if (!std::filesystem::exists(cacheDirectory))
			std::filesystem::create_directories(cacheDirectory);
	}

	const char* VulkanShaderCompiler::GetCacheDirectory()
	{
		return "assets/cache/shader/vulkan";
	}

	bool VulkanShaderCompiler::Compile(const CompileParam& params, CompileOutput& output)
	{
		Timer timer;

		auto shaderSources = PreProcess(params.Source);
		ETH_CORE_TRACE("[Time] preprocess: {0}ms", timer.ElapsedMillis());
		timer.Reset();

		CompileOrGetVulkanBinaries(params, shaderSources, output.ShaderBinary);
		ETH_CORE_TRACE("[Time] compiler: {0}ms", timer.ElapsedMillis());
		timer.Reset();

		return true;
	}

	std::unordered_map<VkShaderStageFlagBits, std::string> VulkanShaderCompiler::PreProcess(const std::string& source)
	{
		ETH_PROFILE_FUNCTION();

		std::unordered_map<VkShaderStageFlagBits, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			ETH_CORE_ASSERT(eol != std::string::npos, "Syntex error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			ETH_CORE_ASSERT(Utils::ShaderTypeFromString(type), "Invalid shader type specification!");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			ETH_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos);
			shaderSources[Utils::ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	void VulkanShaderCompiler::CompileOrGetVulkanBinaries(const CompileParam& params, const std::unordered_map<VkShaderStageFlagBits, std::string>& shaderSources, 
		std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>>& outputBinary)
	{
		ETH_PROFILE_FUNCTION();

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		options.SetWarningsAsErrors();
		options.SetGenerateDebugInfo();
		const bool optimize = false;

		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = GetCacheDirectory();

		for (auto&& [stage, source] : shaderSources)
		{

			std::filesystem::path shaderFilePath = params.FilePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::VkShaderStageCachedFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open() && !params.ForceCompile) {
				ETH_CORE_TRACE("Read shader file from: {0}", shaderFilePath);
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);
				auto& data = outputBinary[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
				in.close();
			}
			else
			{
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::VkShaderStageToShaderC(stage), params.FilePath.c_str(), options);
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					ETH_CORE_ERROR(module.GetErrorMessage());
					ETH_CORE_ASSERT(false);
				}

				outputBinary[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = outputBinary[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}
	}

	void VulkanShaderCompiler::Reflect(const ReflectParam& param, ReflectOutput& outputs)
	{
		// TODO: check binary exist
		for (auto&& [stage, data] : param.ShaderBinary)
		{
			ReflectStage(stage, data, outputs.ShaderDescriptorSets, outputs.PushConstantRanges);
		}
	}

	static std::unordered_map<uint32_t, std::unordered_map<uint32_t, VulkanShaderCompiler::UniformBuffer*>> s_UniformBuffers; // set -> binding point -> buffer

	void VulkanShaderCompiler::ReflectStage(VkShaderStageFlagBits stage, const std::vector<uint32_t>& shaderBinary,
		std::vector<ShaderDescriptorSetData>& shaderDescriptorSets,
		std::vector<VkPushConstantRange>& pushConstantRanges)
	{
		spirv_cross::Compiler compiler(shaderBinary);
		spirv_cross::ShaderResources res = compiler.get_shader_resources();

		// ETH_CORE_TRACE("VulkanShader::Reflect - {0} {1}", Utils::GLShaderStageToString(stage), m_FilePath);

		ETH_CORE_TRACE("    {0} uniform buffers", res.uniform_buffers.size());
		for (const auto& resource : res.uniform_buffers)
		{
			const auto& name = resource.name;
			auto& bufferType = compiler.get_type(resource.base_type_id);
			uint32_t bufferSize = (uint32_t)compiler.get_declared_struct_size(bufferType);
			uint32_t memberCount = (uint32_t)bufferType.member_types.size();
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			uint32_t descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);

			if (descriptorSet >= shaderDescriptorSets.size())
				shaderDescriptorSets.resize(descriptorSet + 1);

			if (s_UniformBuffers[descriptorSet].find(binding) == s_UniformBuffers[descriptorSet].end())
			{
				UniformBuffer* uniformBuffer = new UniformBuffer();
				uniformBuffer->Size = bufferSize;
				uniformBuffer->Name = name;
				uniformBuffer->ShaderStage = VK_SHADER_STAGE_ALL;
				s_UniformBuffers[descriptorSet][binding] = uniformBuffer;
			}
			else
			{
				UniformBuffer* uniformBuffer = s_UniformBuffers[descriptorSet][binding];
				if (bufferSize > uniformBuffer->Size)
					uniformBuffer->Size = bufferSize;

			}

			ShaderDescriptorSetData& shaderDescriptorSet = shaderDescriptorSets[descriptorSet];
			shaderDescriptorSet.UniformBuffers[binding] = s_UniformBuffers[descriptorSet][binding];

			VulkanShaderSystem::ReflectBufferData(descriptorSet, binding, s_UniformBuffers[descriptorSet][binding]);

			ETH_CORE_TRACE("  {0} ({1}, {2})", name, descriptorSet, binding);
			ETH_CORE_TRACE("  Member Count: {0}", memberCount);
			ETH_CORE_TRACE("  Size: {0}", bufferSize);
			ETH_CORE_TRACE("-------------------");
		}



		ETH_CORE_INFO("Sampled Images:");
		for (const auto& resource : res.sampled_images)
		{
			const auto& name = resource.name;
			auto& baseType = compiler.get_type(resource.base_type_id);
			auto& type = compiler.get_type(resource.type_id);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			uint32_t descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
			uint32_t dimension = baseType.image.dim;
			uint32_t arraySize = type.array[0];
			if (arraySize == 0)
				arraySize = 1;
			if (descriptorSet >= shaderDescriptorSets.size())
				shaderDescriptorSets.resize(descriptorSet + 1);

			ShaderDescriptorSetData& shaderDescriptorSet = shaderDescriptorSets[descriptorSet];
			auto& imageSampler = shaderDescriptorSet.ImageSamplers[binding];
			imageSampler.DescriptorSet = descriptorSet;
			imageSampler.Name = name;
			imageSampler.ShaderStage = stage;
			imageSampler.ArraySize = arraySize;

			VulkanShaderSystem::ReflectSamplerData(descriptorSet, binding, shaderDescriptorSets[descriptorSet].ImageSamplers[binding]);

			ETH_CORE_TRACE("  {0} ({1}, {2})", name, descriptorSet, binding);
		}


		ETH_CORE_INFO("Push Constant Buffers:");
		for (const auto& resource : res.push_constant_buffers)
		{
			const auto& bufferName = resource.name;
			auto& bufferType = compiler.get_type(resource.base_type_id);
			uint32_t bufferSize = (uint32_t)compiler.get_declared_struct_size(bufferType);
			uint32_t memberCount = uint32_t(bufferType.member_types.size());
			uint32_t bufferOffset = 0;
			if (pushConstantRanges.size())
				bufferOffset = pushConstantRanges.back().offset + pushConstantRanges.back().size;

			auto& pushConstantRange = pushConstantRanges.emplace_back();
			pushConstantRange.stageFlags = stage;
			pushConstantRange.offset = bufferOffset;
			pushConstantRange.size = bufferSize - bufferOffset;
		}
	}
}
