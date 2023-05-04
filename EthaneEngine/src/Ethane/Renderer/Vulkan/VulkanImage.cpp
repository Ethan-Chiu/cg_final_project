//#include "ethpch.h"
//#include "VulkanContext.h"
//#include "VulkanUtils.h"
//
//#include "VulkanImage.h"
//
//namespace Ethane {
//
//	VulkanImage2D::VulkanImage2D(const VulkanContext* context, ImageSpecification specification, void* buffer)
//		: m_Context(context), m_Specification(specification)
//	{
//		ETH_CORE_ASSERT(m_Specification.Width > 0 && m_Specification.Height > 0);
//		ETH_CORE_TRACE("VulkanImage2D::Invalidate ({0})", m_Specification.DebugName);
//
//		auto device = m_Context->GetDevice()->GetVulkanDevice();
//
//		VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
//		if (m_Specification.Usage == ImageUsage::Attachment)
//		{
//			if (Utils::IsDepthFormat(m_Specification.Format))
//				usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
//			else
//				usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
//		}
//		else if (m_Specification.Usage == ImageUsage::Texture)
//		{
//			usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
//		}
//		else if (m_Specification.Usage == ImageUsage::Storage)
//		{
//			usage |= VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
//		}
//
//		VkFormat vulkanFormat = Utils::VulkanImageFormat(m_Specification.Format);
//
//		// Create Image
//		CreateVulkanImage(device,
//			m_Specification.Width,
//			m_Specification.Height,
//			m_Specification.Mips,
//			m_Specification.Layers,
//			vulkanFormat,
//			VK_IMAGE_TILING_OPTIMAL,
//			usage,
//			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
//
//
//		VkImageAspectFlags aspectMask = Utils::IsDepthFormat(m_Specification.Format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
//		if (m_Specification.Format == ImageFormat::DEPTH24STENCIL8)
//			aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
//
//		// Create Image view
//		CreateImageView(device, vulkanFormat, aspectMask);
//
//	}
//
//	VulkanImage2D::VulkanImage2D(const VulkanContext* context, uint32_t width, uint32_t height, uint32_t mip, uint32_t layers, VkFormat format, VkImageTiling tiling,
//		VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlag, VkImageAspectFlags aspectFlag, bool createView)
//        :m_Context(context)
//	{
//		auto device = m_Context->GetDevice()->GetVulkanDevice();
//		CreateVulkanImage(device, width, height, mip, layers, format, tiling, usage, memoryFlag);
//		if (createView) {
//			CreateImageView(device, format, aspectFlag);
//		}
//	}
//
//	Ref<VulkanImage2D> VulkanImage2D::Create(uint32_t width, uint32_t height, uint32_t mip, uint32_t layers, VkFormat format, VkImageTiling tiling,
//		VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlag, VkImageAspectFlags aspectFlag, bool createView)
//	{
//		return CreateRef<VulkanImage2D>(width, height, mip, layers, format, tiling, usage, memoryFlag, aspectFlag, createView);
//	}
//
//	VulkanImage2D::~VulkanImage2D()
//	{
//		if (m_Info.Image)
//		{
//			Destroy();
//		}
//	}
//
//	void VulkanImage2D::CreateVulkanImage(VkDevice device, uint32_t width, uint32_t height, uint32_t mip, uint32_t layers, VkFormat format, VkImageTiling tiling,
//		VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlag)
//	{
//		// Create Image
//		VkImageCreateInfo imageInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
//		imageInfo.imageType = VK_IMAGE_TYPE_2D;
//		imageInfo.extent.width = width;
//		imageInfo.extent.height = height;
//		imageInfo.extent.depth = 1;
//		imageInfo.mipLevels = mip;
//		imageInfo.arrayLayers = 1;
//		imageInfo.format = format;
//		imageInfo.tiling = tiling;
//		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//		imageInfo.usage = usage;
//		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
//		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//		imageInfo.flags = 0; // Optional
//		VK_CHECK_RESULT(vkCreateImage(device, &imageInfo, nullptr, &m_Info.Image));
//
//		//Allocate image memory
//		VkMemoryRequirements memRequirements;
//		vkGetImageMemoryRequirements(device, m_Info.Image, &memRequirements);
//
//		VkMemoryAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
//		allocInfo.allocationSize = memRequirements.size;
//		allocInfo.memoryTypeIndex = Utils::FindMemoryType(m_Context->GetPhysicalDevice()->GetVulkanPhysicalDevice(), memRequirements.memoryTypeBits, memoryFlag);
//		VK_CHECK_RESULT(vkAllocateMemory(device, &allocInfo, nullptr, &m_ImageMemory));
//
//		// Bind the memory
//		VK_CHECK_RESULT(vkBindImageMemory(device, m_Info.Image, m_ImageMemory, 0));
//	}
//
//	void VulkanImage2D::CreateImageView(VkDevice device, VkFormat format, VkImageAspectFlags aspectMask)
//	{
//		VkImageViewCreateInfo viewInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
//		viewInfo.image = m_Info.Image;
//		viewInfo.viewType = m_Specification.Layers > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
//		viewInfo.format = format;
//		viewInfo.subresourceRange.aspectMask = aspectMask;
//		viewInfo.subresourceRange.baseMipLevel = 0;
//		viewInfo.subresourceRange.levelCount = m_Specification.Mips;
//		viewInfo.subresourceRange.baseArrayLayer = 0;
//		viewInfo.subresourceRange.layerCount = m_Specification.Layers;
//		viewInfo.flags = 0;
//		VK_CHECK_RESULT(vkCreateImageView(device, &viewInfo, nullptr, &m_Info.ImageView));
//	}
//
//
//	void VulkanImage2D::Destroy()
//	{
//		if (m_Info.Image == nullptr)
//			return;
//
//		auto device = m_Context->GetDevice()->GetVulkanDevice();
//
//		if (m_Info.ImageView) {
//			vkDestroyImageView(device, m_Info.ImageView, nullptr);
//			ETH_CORE_WARN("VulkanImage2D::Release ImageView = {0}", (const void*)m_Info.ImageView);
//		}
//
//		if (m_ImageMemory) {
//			vkFreeMemory(device, m_ImageMemory, nullptr);
//			m_ImageMemory = nullptr;
//		}
//
//		if (m_Info.Image)
//			vkDestroyImage(device, m_Info.Image, nullptr);
//
//		m_Info.Image = nullptr;
//		m_Info.ImageView = nullptr;
//	}
//
//	void VulkanImage2D::CopyFromBuffer(VkCommandBuffer cmdBuffer, VkBuffer buffer)
//	{
//		VkBufferImageCopy region{};
//		region.bufferOffset = 0;
//		region.bufferRowLength = 0;
//		region.bufferImageHeight = 0;
//
//		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//		region.imageSubresource.mipLevel = 0;
//		region.imageSubresource.baseArrayLayer = 0;
//		region.imageSubresource.layerCount = 1;
//
//		region.imageOffset = { 0, 0, 0 };
//
//		region.imageExtent.width = m_Specification.Width;
//		region.imageExtent.height = m_Specification.Height;
//		region.imageExtent.depth = 1;
//
//		vkCmdCopyBufferToImage(cmdBuffer, buffer, m_Info.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
//	}
//
//	void VulkanImage2D::TransitionLayout(VkCommandBuffer cmdBuffer, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
//	{
//		VkImageMemoryBarrier barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
//		barrier.oldLayout = oldLayout;
//		barrier.newLayout = newLayout;
//		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//		barrier.image = m_Info.Image;
//
//		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//		barrier.subresourceRange.baseMipLevel = 0;
//		barrier.subresourceRange.levelCount = 1;
//		barrier.subresourceRange.baseArrayLayer = 0;
//		barrier.subresourceRange.layerCount = 1;
//
//		VkPipelineStageFlags sourceStage;
//		VkPipelineStageFlags destinationStage;
//
//		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
//			barrier.srcAccessMask = 0;
//			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//
//			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//		}
//		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
//			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//
//			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//		}
//		else {
//			throw std::invalid_argument("unsupported layout transition!");
//		}
//
//		vkCmdPipelineBarrier(cmdBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
//	}
//
//}
