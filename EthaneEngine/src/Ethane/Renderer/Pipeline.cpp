#include "ethpch.h"
#include "Pipeline.h"

#include "RendererAPI.h"

#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanPipeline.h"

namespace Ethane {

	Ref<Pipeline> Pipeline::Create(const PipelineSpecification& spec)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			ETH_CORE_ASSERT(false, "endererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanPipeline>(spec);
		}

		ETH_CORE_ASSERT(false, "Unknpwn RendererAPI");
		return nullptr;
	}
}
