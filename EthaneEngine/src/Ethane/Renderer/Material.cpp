#include "ethpch.h"
#include "Material.h"

#include "Vulkan/VulkanMaterial.h"

#include "RendererAPI.h"

namespace Ethane {

	Ref<Material> Material::Create(const Shader* shader, const std::string& name)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: return nullptr;
		case RendererAPI::API::Vulkan: return CreateRef<VulkanMaterial>(static_cast<const VulkanShader*>(shader), name);
		}
		ETH_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}
