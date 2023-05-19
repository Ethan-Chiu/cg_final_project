#pragma once

#include "Ethane/Renderer/Texture.h"

#include "VulkanImage.h"

namespace Ethane {

	class VulkanTexture2D : public Texture2D
	{
	public:
//		VulkanTexture2D(const std::string& path);
//		VulkanTexture2D(const VulkanContext* context, TextureSpec spec);
		VulkanTexture2D(const VulkanContext* context, VulkanImage2D* image);
		~VulkanTexture2D() override = default;

		void Cleanup();

		virtual void SetData(void* data, uint32_t size) override;

		// Getter
		virtual uint32_t GetWidth() const override { return m_Width; } 
		virtual uint32_t GetHeight() const override { return m_Height; } 
		const VkDescriptorImageInfo& GetDescriptorImageInfo() const { return m_DescriptorInfo; }
		VkImageView GetImageView() { return m_Image->GetImageInfo().ImageView; }
		VkSampler GetImageSampler() { return m_TextureSampler; }

//		virtual Ref<Image2D> GetImage() const { return std::dynamic_pointer_cast<Image2D>(m_Image); }

		// TODO: remove
//		virtual bool operator==(const TextureOld& other) const { return this->m_Path == ((VulkanTexture2D&)other).m_Path; };
	private:
		void CreateTextureSampler();
		void UpdateDescriptorImageInfo();
	private:
        const VulkanContext* m_Context;
		uint32_t m_Width, m_Height, m_ChannelCount;

		// TODO: remove
//		std::string m_Path{};

        VulkanImage2D* m_Image = nullptr;
		VkSampler m_TextureSampler = nullptr;

		VkDescriptorImageInfo m_DescriptorInfo{};
	};
}
