#include "ethpch.h"
#include "VulkanUniformBuffer.h"

#include "VulkanContext.h"

namespace Ethane {

	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size, uint32_t binding)
		:m_Binding(binding)
	{
        m_Size = size;
        CreateVulkanBuffer(m_Size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true);
	}

	void VulkanUniformBuffer::Cleanup()
	{
        VulkanBuffer::Destroy();
	}

}
