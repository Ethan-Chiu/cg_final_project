#pragma once

#include "Vulkan.h"
#include "Ethane/Renderer/RenderPass.h"

namespace Ethane {

	class VulkanRenderPass : public RenderPass
	{
	public:
		VulkanRenderPass() = default;
		virtual ~VulkanRenderPass() {};

		void Create(VkDevice device, bool hasDepth, VkFormat imageFormat, VkFormat depthFormat);
		void Destroy();
		void Begin(VkCommandBuffer cmdBuffer, uint32_t width, uint32_t height, VkFramebuffer frameBuffer);
		void End(VkCommandBuffer cmdBuffer);

		VkRenderPass GetHandle() const { return m_RenderPass; }
	private:
        VkDevice m_Device;
		VkRenderPass m_RenderPass;
	};

}
