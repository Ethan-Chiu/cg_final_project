#pragma once

#include "Ethane/Renderer/Texture.h"

#include "VulkanImage.h"

namespace Ethane {

	class VulkanTexture2D : public Texture2D
	{
	public:
		VulkanTexture2D(VulkanImage2D* image);
		~VulkanTexture2D() override = default;

        void Destroy() override;

		virtual void SetData(void* data, uint32_t size) override;

		// Getter
		virtual uint32_t GetWidth() const override { return m_Width; } 
		virtual uint32_t GetHeight() const override { return m_Height; } 
		const VkDescriptorImageInfo& GetDescriptorImageInfo() const { return m_DescriptorInfo; }
        VulkanImage2D* GetImage() const { return m_Image; }
		VkImageView GetImageView() { return m_Image->GetImageInfo().ImageView; }
		VkSampler GetImageSampler() { return m_TextureSampler; }
        
	private:
		void CreateTextureSampler();
		void UpdateDescriptorImageInfo();
	private:
		uint32_t m_Width, m_Height, m_ChannelCount;

		// TODO: remove
//		std::string m_Path{};

        VulkanImage2D* m_Image = nullptr;
		VkSampler m_TextureSampler = nullptr;

		VkDescriptorImageInfo m_DescriptorInfo{};
	};
}
