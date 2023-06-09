#include "ethpch.h"
#include "VulkanContext.h"
#include "VulkanUtils.h"

#include "VulkanImage.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"

namespace Ethane {

	VulkanImage2D::VulkanImage2D(ImageSpecification specification, void* buffer, uint32_t size)
		: m_Specification(specification)
	{
		ETH_CORE_ASSERT(m_Specification.Width > 0 && m_Specification.Height > 0);
		ETH_CORE_TRACE("VulkanImage2D::Invalidate ({0})", m_Specification.DebugName);

        if (buffer && size == 0) {
            ETH_CORE_ERROR("size should be greater than 0");
        }
        
        CreateImage();
        
        if (buffer) {
            LoadData(buffer, size);
        }
	}

	VulkanImage2D::VulkanImage2D(uint32_t width, uint32_t height, uint32_t mip, uint32_t layers, VkFormat format, VkImageTiling tiling,
		VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlag, VkImageAspectFlags aspectFlag, bool createView)
	{
		auto device = VulkanContext::GetDevice();
		ImageUtils::CreateVulkanImage(device, width, height, mip, layers, format, tiling, usage, memoryFlag, m_Info, m_ImageMemory);
		if (createView) {
            ImageUtils::CreateImageView(device->GetVulkanDevice(), format, aspectFlag, mip, layers, m_Info);
		}
	}

	Ref<VulkanImage2D> VulkanImage2D::Create(uint32_t width, uint32_t height, uint32_t mip, uint32_t layers, VkFormat format, VkImageTiling tiling,
		VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlag, VkImageAspectFlags aspectFlag, bool createView)
	{
		return CreateRef<VulkanImage2D>(width, height, mip, layers, format, tiling, usage, memoryFlag, aspectFlag, createView);
	}

	VulkanImage2D::~VulkanImage2D()
	{
		if (m_Info.Image)
		{
			Destroy();
		}
	}

	void VulkanImage2D::Destroy()
	{
		if (m_Info.Image == nullptr)
			return;

		auto device = VulkanContext::GetDevice()->GetVulkanDevice();
        vkDeviceWaitIdle(device);
        
		if (m_Info.ImageView) {
			vkDestroyImageView(device, m_Info.ImageView, nullptr);
			ETH_CORE_WARN("VulkanImage2D::Release ImageView = {0}", (const void*)m_Info.ImageView);
		}

		if (m_ImageMemory) {
			vkFreeMemory(device, m_ImageMemory, nullptr);
			m_ImageMemory = nullptr;
		}

		if (m_Info.Image)
			vkDestroyImage(device, m_Info.Image, nullptr);

		m_Info.Image = nullptr;
		m_Info.ImageView = nullptr;
	}

    void VulkanImage2D::Resize(uint32_t width, uint32_t height)
    {
        Destroy();
        m_Specification.Width = width;
        m_Specification.Height = height;
        CreateImage();
    }

    void VulkanImage2D::CreateImage()
    {
        auto device = VulkanContext::GetDevice();

        VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
        if (m_Specification.Usage == ImageUsage::Attachment)
        {
            if (Utils::IsDepthFormat(m_Specification.Format))
                usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            else
                usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        else if (m_Specification.Usage == ImageUsage::Texture)
        {
            usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        }
        else if (m_Specification.Usage == ImageUsage::Storage)
        {
            usage |= VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        }

        VkFormat vulkanFormat = ImageUtils::VulkanImageFormat(m_Specification.Format);

        // Create Image
        ImageUtils::CreateVulkanImage(device,
            m_Specification.Width,
            m_Specification.Height,
            m_Specification.Mips,
            m_Specification.Layers,
            vulkanFormat,
            VK_IMAGE_TILING_OPTIMAL,
            usage,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_Info,
            m_ImageMemory);


        VkImageAspectFlags aspectMask = Utils::IsDepthFormat(m_Specification.Format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
        if (m_Specification.Format == ImageFormat::DEPTH24STENCIL8)
            aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

        // Create Image view
        ImageUtils::CreateImageView(device->GetVulkanDevice(), vulkanFormat, aspectMask, m_Specification.Mips, m_Specification.Layers, m_Info);
    }

    void VulkanImage2D::LoadData(void *buffer, uint32_t size)
    {
        // create staging buffer
        VulkanBuffer stagingBuffer{};
        stagingBuffer.CreateVulkanBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true);
        
        // copy data to staging buffer
        stagingBuffer.SetData(buffer, 0, size, 0, 0);
                
        // transition
        VulkanCommandBuffer commandBuffer{VulkanContext::GetDevice()};
        commandBuffer.AllocateAndBeginSingleUse(QueueFamilyTypes::Graphics);
        TransitionLayout(commandBuffer.GetHandle(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        CopyFromBuffer(commandBuffer.GetHandle(), stagingBuffer.GetHandle());

        VkImageLayout finalImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        if (m_Specification.Usage == ImageUsage::Storage)
            finalImageLayout = VK_IMAGE_LAYOUT_GENERAL;
        
        TransitionLayout(commandBuffer.GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, finalImageLayout);
        commandBuffer.EndSingleUse(QueueFamilyTypes::Graphics);
        
        // cleanup staging buffer
        stagingBuffer.Destroy();
    }

	void VulkanImage2D::CopyFromBuffer(VkCommandBuffer cmdBuffer, VkBuffer buffer)
	{
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };

		region.imageExtent.width = m_Specification.Width;
		region.imageExtent.height = m_Specification.Height;
		region.imageExtent.depth = 1;

		vkCmdCopyBufferToImage(cmdBuffer, buffer, m_Info.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	}

	void VulkanImage2D::TransitionLayout(VkCommandBuffer cmdBuffer, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkImageMemoryBarrier barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_Info.Image;

		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_GENERAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            
            barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        }
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(cmdBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	}
}
