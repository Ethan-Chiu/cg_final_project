#pragma once

#include "Vulkan.h"
#include "VulkanBuffer.h"

namespace Ethane {

	class VulkanUniformBuffer : public VulkanBuffer
	{
	public:
		VulkanUniformBuffer(uint32_t size, uint32_t binding);
		void Cleanup();

        bool Resize(uint32_t new_size) { return VulkanBuffer::Resize(new_size, m_Device->GetGraphicsQueue(), m_Device->GetGraphicsCommandPool()); }
		// Getter
		uint32_t GetBinding() { return m_Binding; }
        uint32_t GetSize() { return m_Size; }
	
	private:
		uint32_t m_Binding = 0;
	};

}
