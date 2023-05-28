#include "ethpch.h"
#include "VulkanMaterial.h"

#include "Ethane/Renderer/Renderer.h"
#include "Ethane/Systems/TextureSystem.h"

#include "VulkanContext.h"
#include "VulkanSwapChain.h"
#include "VulkanTexture.h"
#include "VulkanImage.h"
#include "VulkanTargetImage.h"
#include "VulkanPipeline.h"
#include "VulkanStorageBuffer.h"

#include "ShaderUtils/VulkanShaderSystem.h"

namespace Ethane {

	VulkanMaterial::VulkanMaterial(const VulkanShader* shader, const std::string& name)
		: m_Shader(shader), m_Name(name),
		m_WriteDescriptors(VulkanContext::GetFramesInFlight())
	{
		Init();
	}

	VulkanMaterial::VulkanMaterial(const Material* material, const std::string& name)
		: m_Shader(static_cast<const VulkanShader*>(material->GetShader())),
        m_Name(name),
        m_WriteDescriptors(VulkanContext::GetFramesInFlight())
	{
		if (name.empty())
			m_Name = material->GetName();

        const VulkanMaterial* vulkanMaterial = static_cast<const VulkanMaterial*>(material);
	}

	VulkanMaterial::~VulkanMaterial()
	{
	}

    void VulkanMaterial::Destroy()
    {
        VkDevice device = VulkanContext::GetDevice()->GetVulkanDevice();
        vkDeviceWaitIdle(device);
        vkDestroyDescriptorPool(device, m_Pool, nullptr);
    }

	void VulkanMaterial::Init()
	{
		m_MaterialFlags |= (uint32_t)MaterialFlag::DepthTest;
		m_MaterialFlags |= (uint32_t)MaterialFlag::Blend;

        m_ResourceId = VulkanShaderSystem::RegisterShaderInstance(m_Shader);
        
		Invalidate();
	}

	void VulkanMaterial::Invalidate()
	{
		ETH_CORE_TRACE("VulkanMaterial::Invalidate ({0})", m_Name);

		uint32_t framesInFlight = VulkanContext::GetFramesInFlight();

        m_DescriptorSets.resize(framesInFlight);
        
		m_ResourceBindings.clear();
		const auto& WDSetsBase = m_Shader->RetrieveWriteDescriptorSetsBase();
		const uint32_t maxSet = (uint32_t)(WDSetsBase.size());
        if (maxSet == 0)
            return;
		for (uint32_t set = 0; set < maxSet; ++set) {
			const auto& WdsBase = WDSetsBase[set];
			for (auto&& [name, wdsMeta] : WdsBase)
			{
                ResourceBinding& resource = m_ResourceBindings.emplace_back();
				resource.Name = name;
				resource.Set = set;

				VkWriteDescriptorSet wds = wdsMeta.WriteDescriptor;
				if(wds.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
				{
                    resource.CacheBufferInfo = VulkanShaderSystem::GetUniformBufferInfo(set, wds.dstBinding, this);
					for (uint32_t frame = 0; frame < framesInFlight; ++frame) {
						wds.dstArrayElement = 0;
						wds.pBufferInfo = &resource.CacheBufferInfo;
						resource.WriteDescriptors.push_back(wds);
					}

				} else if(wds.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
				{
                    auto tex = TextureSystem::GetDefaultTexture();
                    resource.CacheImageInfos.push_back(std::dynamic_pointer_cast<VulkanTexture2D>(tex)->GetDescriptorImageInfo());
					for (uint32_t frame = 0; frame < framesInFlight; ++frame) {
						wds.dstArrayElement = 0;
                        wds.pImageInfo = &resource.CacheImageInfos[0];
						resource.WriteDescriptors.push_back(wds);
					}
				} else if(wds.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
                {
                    for (uint32_t frame = 0; frame < framesInFlight; ++frame) {
                        wds.dstArrayElement = 0;
                        wds.pBufferInfo = nullptr;
                        resource.WriteDescriptors.push_back(wds);
                    }
                } else if(wds.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
                {
                    for (uint32_t frame = 0; frame < framesInFlight; ++frame) {
                        wds.dstArrayElement = 0;
                        wds.pImageInfo = nullptr;
                        resource.WriteDescriptors.push_back(wds);
                    }
                }
			}
		}

		m_Pool = m_Shader->CreateDescriptorPool(framesInFlight);
		for (uint32_t frame = 0; frame < framesInFlight; ++frame) {
			for (uint32_t set = 0; set < maxSet; ++set) {
				m_DescriptorSets[frame].push_back(m_Shader->CreateDescriptorSet(set, m_Pool));
			}
		}
	}

	void VulkanMaterial::ApplyMaterial()
	{
		auto device = VulkanContext::GetDevice()->GetVulkanDevice();
		uint32_t frameIndex = VulkanContext::GetSwapchain()->GetCurrentFrameIndex();
        uint32_t iamgeIndex = VulkanContext::GetSwapchain()->GetCurrentImageIndex();

		std::vector<VkDescriptorImageInfo> arrayImageInfos;
		std::vector<VkWriteDescriptorSet> writeDescriptors{};
        
		// uniform buffer and texture sampler
		for (auto&& resource : m_ResourceBindings) {
            auto& wds = resource.WriteDescriptors[frameIndex];
            
            // if point to different resources based on currrent image index
            if (resource.CacheImageInfos.size() > 1)
            {
                wds.dstArrayElement = 0;
                wds.pImageInfo = &(resource.CacheImageInfos[iamgeIndex]);
            }
            else if (resource.CacheBufferInfo.buffer != nullptr)
            {
                wds.dstArrayElement = 0;
                wds.pBufferInfo = &(resource.CacheBufferInfo);
            }
            writeDescriptors.emplace_back(wds).dstSet = m_DescriptorSets[frameIndex][resource.Set];
		}
		vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptors.size()), writeDescriptors.data(), 0, nullptr);

		m_WriteDescriptors[frameIndex].clear();
		m_DescriptorArrays.clear();
	}

    bool VulkanMaterial::SetImage(const std::string& name, const TargetImage* image)
    {
        bool found = false;
        for(auto& resource: m_ResourceBindings)
        {
            if(resource.Name == name){
                found = true;
                uint32_t imageCount = VulkanContext::GetSwapchain()->GetImageCount();
                const VulkanTargetImage* vulkanTargetImage = dynamic_cast<const VulkanTargetImage*>(image);
                resource.CacheImageInfos.clear();
                resource.CacheImageInfos.resize(imageCount);
                for (uint32_t img = 0; img < imageCount; ++img) {
                    resource.CacheImageInfos[img] = vulkanTargetImage->GetDescriptorImageInfo(img);
                }
                break;
            }
        }
        ETH_CORE_ASSERT(found, "resource not found");
        return found;
    }

    bool VulkanMaterial::SetImage(const std::string& name, const Texture2D* image)
    {
        bool found = false;
        for(auto& resource: m_ResourceBindings)
        {
            if(resource.Name == name){
                found = true;
                uint32_t framesInFlight = VulkanContext::GetFramesInFlight();
                const VulkanTexture2D* vulkantexture = dynamic_cast<const VulkanTexture2D*>(image);
                resource.CacheImageInfos.clear();
                resource.CacheImageInfos.push_back(vulkantexture->GetDescriptorImageInfo());
                for (uint32_t frame = 0; frame < framesInFlight; ++frame) {
                    auto& wds = resource.WriteDescriptors[frame];
                    wds.dstArrayElement = 0;
                    wds.pImageInfo = &resource.CacheImageInfos[0];
                }
                break;
            }
        }
        ETH_CORE_ASSERT(found, "resource not found");
        return found;
    }

    bool VulkanMaterial::SetData(const std::string& name, const StorageBuffer* buffer)
    {
        bool found = false;
        for(auto& resource: m_ResourceBindings)
        {
            if(resource.Name == name){
                found = true;
                uint32_t framesInFlight = VulkanContext::GetFramesInFlight();
                resource.CacheBufferInfo = dynamic_cast<const VulkanStorageBuffer*>(buffer)->GetBufferInfo();
                break;
            }
        }
        ETH_CORE_ASSERT(found, "resource not found");
        return found;
    }
    
}
