#pragma once

#include "VulkanContext.h"
#include "Ethane/Renderer/RenderCommandBuffer.h"
#include "VulkanCommandBuffer.h"

namespace Ethane {

	class VulkanRenderCommandBuffer : public RenderCommandBuffer
	{
	public:
		VulkanRenderCommandBuffer(bool isInFlight, const std::string& debugName = "");
		~VulkanRenderCommandBuffer() = default;
        
        void Destroy() override;
        
		void Begin() override;
        void End() override;
        void Wait() override;
        void Submit() override;

		// Getter
        const VulkanCommandBuffer* GetCommandBuffer() const;

	private:
		std::string m_DebugName;
        bool m_IsInFlight;
		VkCommandPool m_CommandPool = nullptr;
		std::vector<VulkanCommandBuffer> m_CommandBuffers;
		std::vector<VkFence> m_WaitFences;
	};

}
