//#pragma once
//
//#include "Ethane/Renderer/Image.h"
//
//#include <vulkan/vulkan.h>
//
//namespace Ethane {
//
//	struct VulkanImageInfo
//	{
//		VkImage Image = nullptr;
//		VkImageView ImageView = nullptr;
//	};
//
//	class VulkanImage2D  : public Image2D
//	{
//	public:
//		VulkanImage2D(const VulkanContext* context, ImageSpecification specification, void* buffer = nullptr);
//		virtual ~VulkanImage2D() override;
//
//		VulkanImage2D(const VulkanContext* context, uint32_t width, uint32_t height, uint32_t mip, uint32_t layers,
//                      VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlag,
//                      VkImageAspectFlags aspectFlag, bool createView);
//
//		static Ref<VulkanImage2D> Create(uint32_t width, uint32_t height, uint32_t mip, uint32_t layers, VkFormat format,
//			VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlag, VkImageAspectFlags aspectFlag, bool createView);
//
//		virtual void Destroy() override;
//
//		void CopyFromBuffer(VkCommandBuffer cmdBuffer, VkBuffer buffer);
//		void TransitionLayout(VkCommandBuffer cmdBuffer, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
//
//		// Getter
//		virtual uint32_t GetWidth() const override { return m_Specification.Width; }
//		virtual uint32_t GetHeight() const override { return m_Specification.Height; }
//		virtual float GetAspectRatio() const override { return (float)m_Specification.Width / (float)m_Specification.Height; }
//		virtual ImageSpecification& GetSpecification() override { return m_Specification; }
//		virtual const ImageSpecification& GetSpecification() const override { return m_Specification; }
//
//		VulkanImageInfo& GetImageInfo() { return m_Info; }
//		const VulkanImageInfo& GetImageInfo() const { return m_Info; }
//
//	private:
//		void CreateVulkanImage(VkDevice device, uint32_t width, uint32_t height, uint32_t mip, uint32_t layers, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlag);
//		void CreateImageView(VkDevice device, VkFormat format, VkImageAspectFlags aspectMask);
//
//	private:
//        const VulkanContext* m_Context;
//
//		ImageSpecification m_Specification;
//		VulkanImageInfo m_Info;
//		VkDeviceMemory m_ImageMemory;
//	};
//
//
//	namespace Utils {
//
//		inline VkFormat VulkanImageFormat(ImageFormat format)
//		{
//			switch (format)
//			{
//			case ImageFormat::RED32F:          return VK_FORMAT_R32_SFLOAT;
//			case ImageFormat::RG16F:		   return VK_FORMAT_R16G16_SFLOAT;
//			case ImageFormat::RG32F:		   return VK_FORMAT_R32G32_SFLOAT;
//			case ImageFormat::RGBA:            return VK_FORMAT_R8G8B8A8_UNORM;
//			case ImageFormat::RGBA16F:         return VK_FORMAT_R16G16B16A16_SFLOAT;
//			case ImageFormat::RGBA32F:         return VK_FORMAT_R32G32B32A32_SFLOAT;
//			case ImageFormat::DEPTH32F:        return VK_FORMAT_D32_SFLOAT;
//			case ImageFormat::DEPTH24STENCIL8: return VK_FORMAT_D24_UNORM_S8_UINT;// VulkanContext::GetDevice()->GetPhysicalDevice()->GetDepthFormat();
//			}
//			ETH_CORE_ASSERT(false);
//			return VK_FORMAT_UNDEFINED;
//		}
//
//	}
//
//}
