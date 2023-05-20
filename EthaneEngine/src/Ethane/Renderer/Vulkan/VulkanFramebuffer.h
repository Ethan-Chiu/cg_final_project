#pragma once

#include "Ethane/Renderer/Framebuffer.h"

#include "Vulkan.h"

#include "VulkanImage.h"

#include "VulkanRenderPass.h"

//#include "VulkanTexture.h"

namespace Ethane {

	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const VulkanContext* ctx, const FramebufferSpecification& spec);
        virtual void Destroy() override;
        
		virtual void Resize(uint32_t width, uint32_t height) override;

		void Invalidate();

		// Getter
		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
		uint32_t GetWidth()  const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

        VkFramebuffer GetVulkanFramebuffer() const { return m_Framebuffer; }
        virtual const VulkanRenderPass* GetRenderPass() const override { return m_RenderPass.get(); }

//		virtual Ref<Image2D> GetImage(uint32_t attachmentIndex = 0) const { ETH_CORE_ASSERT(attachmentIndex < m_AttachmentImages.size()); return m_AttachmentImages[attachmentIndex]; }
//		virtual Ref<VulkanTexture2D> GetTexture(uint32_t attachmentIndex = 0) const { ETH_CORE_ASSERT(attachmentIndex < m_AttachmentTextures.size()); return m_AttachmentTextures[attachmentIndex]; }
//		virtual Ref<Image2D> GetDepthImage() const { return m_DepthAttachmentImage; }

		const std::vector<VkClearValue>& GetVulkanClearValues() const { return m_ClearValues; }
        
        size_t GetColorAttachmentCount() const { return m_Specification.SwapChainTarget ? 1 : m_AttachmentImages.size(); }
		bool HasDepthAttachment() const { return (bool)m_DepthAttachmentImage; }

	private:
		FramebufferSpecification m_Specification;
		uint32_t m_Width = 0, m_Height = 0;
        
        const VulkanContext* m_Context;

		std::vector<VulkanImage2D*> m_AttachmentImages;
        VulkanImage2D* m_DepthAttachmentImage;
        
        std::vector<ImageFormat> m_AttachmentFormats;
        ImageFormat m_DepthAttachmentFormat = ImageFormat::None;
//		std::vector<Ref<VulkanTexture2D>> m_AttachmentTextures;
        
		std::vector<VkClearValue> m_ClearValues;

		Scope<VulkanRenderPass> m_RenderPass = nullptr;
		VkFramebuffer m_Framebuffer = nullptr;

		std::vector<std::function<void(Ref<Framebuffer>)>> m_ResizeCallbacks;
	};

}
