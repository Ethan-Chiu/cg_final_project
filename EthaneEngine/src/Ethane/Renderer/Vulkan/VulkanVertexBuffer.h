#pragma once

#include "Ethane/Renderer/VertexBuffer.h"

#include "VulkanContext.h"
#include "VulkanBuffer.h"

namespace Ethane {

	class VulkanVertexBuffer : public VertexBuffer, public VulkanBuffer
	{
	public:
		VulkanVertexBuffer(uint32_t size); // , VertexBufferUsage usage = VertexBufferUsage::Dynamic
		VulkanVertexBuffer(void* data, uint32_t size); // , VertexBufferUsage usage = VertexBufferUsage::Static

		void Destroy() override;

		virtual void SetData(const void* data, uint32_t size) override { SetData(data, size, 0); };
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0);
		
		// Getter
		VkBuffer GetVulkanBuffer() const { return m_Buffer; }
	};

}
