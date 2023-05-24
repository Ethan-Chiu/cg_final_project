#pragma once

#include "Vulkan.h"
#include "VulkanRenderPass.h"

namespace Ethane {

	class VulkanFramebuffer
	{
	public:
		VulkanFramebuffer(const VulkanDevice* device, const VulkanRenderPass* renderpass);
        void Destroy();

		void Invalidate(uint32_t width, uint32_t height, const std::vector<VkImageView>& imageViews);

		// Getter
        VkFramebuffer GetHandle() const { return m_Framebuffer; }
        const VulkanRenderPass* GetRenderPass() const { return m_RenderPass; }
	private:
        const VulkanDevice* m_Device;
		const VulkanRenderPass* m_RenderPass = nullptr;
        
		VkFramebuffer m_Framebuffer = nullptr;
	};

}
