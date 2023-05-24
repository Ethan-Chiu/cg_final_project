#include "ethpch.h"
#include "RendererAPI.h"
#include "Shader.h"

#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanShader.h"

namespace Ethane {

	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			ETH_CORE_ASSERT(false, "endererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanShader>(filepath);
		}

		ETH_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			ETH_CORE_ASSERT(false, "endererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanShader>(name, vertexSrc, fragmentSrc);
		}

		ETH_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	std::string Shader::ReadFile(const std::string& filepath)
	{
		ETH_PROFILE_FUNCTION();

		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
		}
		else
		{
			ETH_CORE_ERROR("Could not open file '{0}'", filepath);
		}
		in.close();
		return result;
	}

}
