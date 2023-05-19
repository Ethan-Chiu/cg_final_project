#pragma once

#include "VulkanContext.h"
#include "VulkanCommandBuffer.h"

namespace Ethane {

	class VulkanRenderCommandBuffer
	{
	public:
		VulkanRenderCommandBuffer(const VulkanContext* ctx, uint32_t count = 0, std::string debugName = "");
		~VulkanRenderCommandBuffer() = default;
        
        void Destroy();
        
		void Begin();
        void End();
        void Submit();

		// Getter
		const VulkanCommandBuffer* GetCommandBuffer(uint32_t frameIndex) const
		{
			ETH_CORE_ASSERT(frameIndex < m_CommandBuffers.size());
			return &m_CommandBuffers[frameIndex];
		}

	private:
		std::string m_DebugName;
        const VulkanContext* m_Context;
		VkCommandPool m_CommandPool = nullptr;
		std::vector<VulkanCommandBuffer> m_CommandBuffers;
		std::vector<VkFence> m_WaitFences;
	};

}
