#include "ethpch.h"
#include "VulkanVertexBuffer.h"

#include "VulkanContext.h"

namespace Ethane {

	VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size)
        :VulkanBuffer()
	{
		m_Size = size;
		CreateVulkanBuffer(m_Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(void* data, uint32_t size)
        :VulkanBuffer()
	{
		m_Size = size;

		// create staging buffer
        VulkanBuffer stagingBuffer{};
		stagingBuffer.CreateVulkanBuffer(m_Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true);
		
		// copy data to staging buffer
		stagingBuffer.SetData(data, 0, size, 0, 0);
				
		// create vertex buffer (gpu local memory)
		CreateVulkanBuffer(m_Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true);

		// copy from staging buffer
		VulkanBuffer::CopyTo(m_Device, m_Device->GetGraphicsCommandPool(), 0, m_Device->GetGraphicsQueue(), stagingBuffer.GetHandle(), 0, m_Buffer, 0, m_Size);
				
		// cleanup staging buffer
		stagingBuffer.Destroy();
	}

	void VulkanVertexBuffer::Destroy()
	{
		VulkanBuffer::Destroy();
	}


	void VulkanVertexBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		// create staging buffer
        VulkanBuffer stagingBuffer{};
		stagingBuffer.CreateVulkanBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true);

		// copy data to staging buffer
		stagingBuffer.SetData(data, offset, size, 0, 0);

		// copy from staging buffer
		VulkanBuffer::CopyTo(m_Device, m_Device->GetGraphicsCommandPool(), 0, m_Device->GetGraphicsQueue(), stagingBuffer.GetHandle(), 0, m_Buffer, 0, size);

		// cleanup staging buffer
		stagingBuffer.Destroy();
	}

}
