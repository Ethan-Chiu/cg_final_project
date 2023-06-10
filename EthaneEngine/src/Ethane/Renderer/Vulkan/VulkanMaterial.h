#pragma once

#include "Ethane/Renderer/Material.h"

#include "VulkanShader.h"

// TODO: temp remove 
#include "VulkanTexture.h"

namespace Ethane {

	class VulkanMaterial : public Material
	{
	public:
		VulkanMaterial(const VulkanShader* shader, const std::string& name);
		VulkanMaterial(const Material* material, const std::string& name);
		virtual ~VulkanMaterial();

        virtual void Destroy() override;
        
		virtual void Invalidate() override;

		template <typename T>
		void Set(const std::string& name, const T& value)
		{
		}

		template<typename T>
		T& Get(const std::string& name)
		{
			return *(T*)(0);
		}

		void ApplyMaterial();

        virtual bool SetImage(const std::string& name, const TargetImage* image) override;
        virtual bool SetImage(const std::string& name, const Image2D* image, ImageLayout layout) override;
        virtual bool SetImage(const std::string& name, const Texture2D* image) override;
        
        virtual bool SetData(const std::string& name, const StorageBuffer* buffer) override;
        
		// Getter
		virtual uint32_t GetFlags() const override { return m_MaterialFlags; }
		virtual bool HasFlag(MaterialFlag flag) const { return (uint32_t)flag & m_MaterialFlags; }
		virtual const Shader* GetShader() const override { return m_Shader; }
		virtual const std::string& GetName() const override { return m_Name; }
        
		VkDescriptorSet GetDescriptorSet(uint32_t frameIndex) const { return !m_DescriptorSets.empty() ? m_DescriptorSets[frameIndex][0] : nullptr; }
		std::vector<VkDescriptorSet> GetDescriptorSets(uint32_t frameIndex) const { return m_DescriptorSets[frameIndex]; }
        uint32_t GetResourceId() const { return m_ResourceId; }
		

	private:
		void Init();

	private:
		std::string m_Name;
		uint32_t m_MaterialFlags = 0;
		
        const VulkanShader* m_Shader;
        uint32_t m_ResourceId;

		// resource binding map
		struct ResourceBinding {
			std::string Name;
			uint32_t Set;
            VkDescriptorBufferInfo CacheBufferInfo;
            std::vector<VkDescriptorImageInfo> CacheImageInfos;
			std::vector<VkWriteDescriptorSet> WriteDescriptors; // frame
		};
		std::vector<ResourceBinding> m_ResourceBindings;

		VkDescriptorPool m_Pool;
		std::vector < std::vector < VkDescriptorSet>> m_DescriptorSets; // frame -> set

		// Write Descriptors
		std::unordered_map<uint32_t, VkWriteDescriptorSet> m_DescriptorArrays; // binding
		std::vector<std::vector<VkWriteDescriptorSet>> m_WriteDescriptors; // frame -> set
	};

}
