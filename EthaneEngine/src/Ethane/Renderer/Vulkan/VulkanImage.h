#pragma once

#include "Ethane/Renderer/Image.h"
#include "VulkanContext.h"
#include "VulkanImageUtils.h"

namespace Ethane {

	class VulkanImage2D  : public Image2D
	{
	public:
		VulkanImage2D(ImageSpecification specification, void* buffer = nullptr);
		virtual ~VulkanImage2D() override;

		VulkanImage2D(uint32_t width, uint32_t height, uint32_t mip, uint32_t layers,
                      VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlag,
                      VkImageAspectFlags aspectFlag, bool createView);

		static Ref<VulkanImage2D> Create(uint32_t width, uint32_t height, uint32_t mip, uint32_t layers, VkFormat format,
			VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlag, VkImageAspectFlags aspectFlag, bool createView);

		virtual void Destroy() override;
        
        void Resize(uint32_t width, uint32_t height);

		void CopyFromBuffer(VkCommandBuffer cmdBuffer, VkBuffer buffer);
		void TransitionLayout(VkCommandBuffer cmdBuffer, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

		// Getter
		virtual uint32_t GetWidth() const override { return m_Specification.Width; }
		virtual uint32_t GetHeight() const override { return m_Specification.Height; }
		virtual float GetAspectRatio() const override { return (float)m_Specification.Width / (float)m_Specification.Height; }
		virtual ImageSpecification& GetSpecification() override { return m_Specification; }
		virtual const ImageSpecification& GetSpecification() const override { return m_Specification; }

		ImageUtils::VulkanImageInfo& GetImageInfo() { return m_Info; }
		const ImageUtils::VulkanImageInfo& GetImageInfo() const { return m_Info; }

    private:
        void CreateImage();
        
	private:
		ImageSpecification m_Specification;
        ImageUtils::VulkanImageInfo m_Info;
		VkDeviceMemory m_ImageMemory;
	};
}
