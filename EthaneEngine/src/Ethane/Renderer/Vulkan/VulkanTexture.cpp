#include "ethpch.h"
#include "VulkanTexture.h"

#include "VulkanContext.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"

namespace Ethane{

//	VulkanTexture2D::VulkanTexture2D(const std::string& path)
//		:m_Path(path)
//	{
//		// TODO: Invalidate
//		ETH_PROFILE_FUNCTION();
//
//		int width, height, channels;
//		stbi_uc* data = nullptr;
//		{
//			ETH_PROFILE_SCOPE("stbi_load _ OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
//			data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
//		}
//		ETH_CORE_ASSERT(data, "Failed to load image!");
//		m_Width = width;
//		m_Height = height;
//		VkDeviceSize imageSize = m_Width * m_Height * 4;
//		ETH_CORE_TRACE("w: {0}, h: {1}, ch: {2}", m_Width, m_Height, channels);
//
//		auto device = VulkanContext::GetDevice()->GetVulkanDevice();
//
//		// Create Imgae
//		ImageSpecification imageSpec;
//		imageSpec.Width = m_Width;
//		imageSpec.Height = m_Height;
//		m_Image = CreateRef<VulkanImage2D>(imageSpec);
//
//		SetData(data ,imageSize);
//		stbi_image_free(data);
//
//		// create sampler
//		CreateTextureSampler();
//
//		UpdateDescriptorImageInfo();
//	}


	VulkanTexture2D::VulkanTexture2D(VulkanImage2D* image)
		:m_Width(image->GetWidth()), m_Height(image->GetHeight()), m_ChannelCount(4)
	{
        m_Image = image;

		CreateTextureSampler();

		UpdateDescriptorImageInfo();
	}

    void VulkanTexture2D::Destroy()
    {
        auto vk_device = VulkanContext::GetDevice()->GetVulkanDevice();

        vkDeviceWaitIdle(vk_device);

        m_Image->Destroy();

        if (m_TextureSampler)
            vkDestroySampler(vk_device, m_TextureSampler, nullptr);
    }

	void VulkanTexture2D::SetData(void* data, uint32_t imageSize)
	{
		const auto device = VulkanContext::GetDevice();

		// create staging buffer
        VulkanBuffer stagingBuffer{};
		stagingBuffer.CreateVulkanBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true);

		// copy data to staging buffer
		stagingBuffer.SetData(data, 0, imageSize, 0, 0);
	
		// transition
		VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;
        VulkanCommandBuffer commandBuffer{device};
		commandBuffer.AllocateAndBeginSingleUse(QueueFamilyTypes::Graphics);
		m_Image->TransitionLayout(commandBuffer.GetHandle(), format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		m_Image->CopyFromBuffer(commandBuffer.GetHandle(), stagingBuffer.GetHandle());

		m_Image->TransitionLayout(commandBuffer.GetHandle(), format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		commandBuffer.EndSingleUse(QueueFamilyTypes::Graphics);

		// cleanup staging buffer
		stagingBuffer.Destroy();
	}

	void VulkanTexture2D::CreateTextureSampler() {
        auto vk_device = VulkanContext::GetDevice()->GetVulkanDevice();
		
		VkSamplerCreateInfo samplerInfo{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 16;
		
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
		
		VK_CHECK_RESULT(vkCreateSampler(vk_device, &samplerInfo, nullptr, &m_TextureSampler));
	}

	void VulkanTexture2D::UpdateDescriptorImageInfo()
	{
		//TODO: fix
		//if (m_Specification.Format == ImageFormat::DEPTH24STENCIL8 || m_Specification.Format == ImageFormat::DEPTH32F)
		//	m_DescriptorInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		//else if (m_Specification.Usage == ImageUsage::Storage)
		//	m_DescriptorInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		//else
			m_DescriptorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;


		m_DescriptorInfo.imageView = m_Image->GetImageInfo().ImageView;
		m_DescriptorInfo.sampler = m_TextureSampler;

		ETH_CORE_ASSERT(m_DescriptorInfo.imageView != VK_NULL_HANDLE, "image view is null");
		ETH_CORE_TRACE("VulkanImage2D::UpdateDescriptorImageInfo to ImageView = {0}", (const void*)m_DescriptorInfo.imageView);
	}
}
