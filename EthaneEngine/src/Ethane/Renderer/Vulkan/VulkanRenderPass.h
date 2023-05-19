#pragma once

#include "Ethane/Renderer/RenderPass.h"
#include "VulkanImage.h"

namespace Ethane {

	class VulkanRenderPass : public RenderPass
	{
	public:
		VulkanRenderPass(VkDevice device, const std::vector<ImageFormat>& colorAttachmentFormats, ImageFormat depthAttachmentFormat, bool clearOnLoad, bool finalPresent = false);
		virtual ~VulkanRenderPass() {};

        static Scope<VulkanRenderPass> Create(VkDevice device, const std::vector<ImageFormat>& colorAttachmentFormats, ImageFormat depthAttachmentFormat, bool clearOnLoad, bool finalPresent = false);
        
		void CreateOld(VkDevice device, bool hasDepth, VkFormat imageFormat, VkFormat depthFormat);
		void Destroy();
        
		void Begin(VkCommandBuffer cmdBuffer, uint32_t width, uint32_t height, VkFramebuffer frameBuffer);
		void End(VkCommandBuffer cmdBuffer);

		VkRenderPass GetHandle() const { return m_RenderPass; }
	private:
        VkDevice m_Device;
		VkRenderPass m_RenderPass;
	};

}
