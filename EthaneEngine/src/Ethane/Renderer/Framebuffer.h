#pragma once

#include "Ethane/Core/Base.h"
#include "Image.h"
#include <map>

namespace Ethane {

	class Framebuffer;

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(Image2D* image)
			: AttachImage(image) {}

        Image2D* AttachImage;
		// TODO: filtering/wrap
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: Attachments(attachments) {}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
        std::string DebugName = "framebuffer";
        
		uint32_t Width, Height;
		glm::vec4 ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		
        FramebufferAttachmentSpecification Attachments;
        
		bool SwapChainTarget = false;
		bool ClearOnLoad = true;
		bool Blend = true;
	};

	class Framebuffer 
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual void Destroy() = 0;
        
		// Getter
		virtual const FramebufferSpecification& GetSpecification() const = 0;

		static Ref<Framebuffer> Create(const GraphicsContext* ctx, const FramebufferSpecification& spec);
	};

}
