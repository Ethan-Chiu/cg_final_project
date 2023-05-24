#pragma once

#include "Ethane/Renderer/IndexBuffer.h"

#include "VulkanContext.h"
#include "VulkanBuffer.h"

namespace Ethane {

	class VulkanIndexBuffer : public IndexBuffer, public VulkanBuffer
	{
	public:
		VulkanIndexBuffer(uint32_t size); // size in byte
		VulkanIndexBuffer(void* data, uint32_t size = 0);
		virtual ~VulkanIndexBuffer();

		void Destroy();

		void SetData(void* buffer, uint32_t size, uint32_t offset = 0) override;

		//Getter
		virtual uint32_t GetCount() const override { return m_Size / sizeof(uint32_t); }
		virtual uint32_t GetSize() const { return m_Size; }
		VkBuffer GetVulkanBuffer() { return m_Buffer; } // TODO: REMOVE use GetHandle instead

	private:
		// Buffer m_LocalData;
	};

}
