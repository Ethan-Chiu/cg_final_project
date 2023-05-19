#include "ethpch.h"
#include "VulkanRendererAPI.h"
#include "VulkanContext.h"
#include "VulkanSwapChain.h"

#include "VulkanPipeline.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"

#include "./ShaderUtils/VulkanShaderSystem.h"

// #include "VulkanShader.h"
//#include "VulkanTexture.h"

namespace Ethane {

	VulkanRendererAPI::VulkanRendererData* VulkanRendererAPI::s_Data = nullptr;

	void VulkanRendererAPI::Init(const RendererConfig& config, const GraphicsContext* ctx)
	{
		s_Data = new VulkanRendererData();

        m_Context = static_cast<const VulkanContext*>(ctx);
        auto imageCount = m_Context->GetSwapchain()->GetImageCount();
        ETH_INFO("{0} images in swapchain", imageCount);
        
        s_Data->DescriptorPools.resize(imageCount);
        s_Data->DescriptorPoolAllocationCount.resize(imageCount);
        
		// Create descriptor pools
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 100000;
		pool_info.poolSizeCount = (uint32_t)(std::end(pool_sizes)-std::begin(pool_sizes));
		pool_info.pPoolSizes = pool_sizes;
		VkDevice device = m_Context->GetDevice()->GetVulkanDevice();
		for (uint32_t i = 0; i < imageCount; i++)
		{
			VK_CHECK_RESULT(vkCreateDescriptorPool(device, &pool_info, nullptr, &s_Data->DescriptorPools[i]));
			s_Data->DescriptorPoolAllocationCount[i] = 0;
		}

		VulkanShaderSystem::Init();

		// Create fullscreen quad
		float x = -1;
		float y = -1;
		float width = 2, height = 2;
		struct QuadVertex
		{
			glm::vec3 Position;
			glm::vec2 TexCoord;
		};

		QuadVertex* data = new QuadVertex[4];

		data[0].Position = glm::vec3(x, y, 0.0f);
		data[0].TexCoord = glm::vec2(0, 0);

		data[1].Position = glm::vec3(x + width, y, 0.0f);
		data[1].TexCoord = glm::vec2(1, 0);

		data[2].Position = glm::vec3(x + width, y + height, 0.0f);
		data[2].TexCoord = glm::vec2(1, 1);

		data[3].Position = glm::vec3(x, y + height, 0.0f);
		data[3].TexCoord = glm::vec2(0, 1);

		s_Data->QuadVertexBuffer = CreateRef<VulkanVertexBuffer>(m_Context, data, 4 * sizeof(QuadVertex));
		uint32_t indices[6] = { 0, 1, 2, 2, 3, 0, };
		s_Data->QuadIndexBuffer = CreateRef<VulkanIndexBuffer>(m_Context, indices, 6 * sizeof(uint32_t));
        
        m_RenderCommandBuffer = CreateScope<VulkanRenderCommandBuffer>(m_Context);
	}

    void VulkanRendererAPI::Shutdown()
    {
        m_RenderCommandBuffer->Destroy();
        
        s_Data->QuadVertexBuffer->Destroy();
        s_Data->QuadIndexBuffer->Destroy();
        
        auto imageCount = m_Context->GetSwapchain()->GetImageCount();
        for (auto pool : s_Data->DescriptorPools)
        {
            vkDestroyDescriptorPool(m_Context->GetDevice()->GetVulkanDevice(), pool, nullptr);
        }
        s_Data->DescriptorPools.clear();

        
        delete s_Data;
    }

//	VkDescriptorSet VulkanRendererAPI::AllocateDescriptorSet(VkDescriptorSetAllocateInfo& allocInfo)
//	{
//		ETH_PROFILE_FUNCTION();
//
//		uint32_t frameIndex = VulkanContext::GetSwapChain()->GetCurrentFrameIndex(); // Renderer::GetCurrentFrameIndex();
//		allocInfo.descriptorPool = s_Data->DescriptorPools[frameIndex];
//		VkDevice device = VulkanContext::GetDevice()->GetVulkanDevice();
//		VkDescriptorSet result;
//		VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, &result));
//		s_Data->DescriptorPoolAllocationCount[frameIndex] += allocInfo.descriptorSetCount;
//		return result;
//	}

	void VulkanRendererAPI::BeginFrame()
	{
		VkDevice device = m_Context->GetDevice()->GetVulkanDevice();
		uint32_t frameIndex = m_Context->GetSwapchain()->GetCurrentFrameIndex();
	}

	void VulkanRendererAPI::BeginRenderCommandBuffer()
	{
        m_RenderCommandBuffer->Begin();
	}

	void VulkanRendererAPI::EndRenderCommandBuffer()
	{
        m_RenderCommandBuffer->End();
	}

	void VulkanRendererAPI::BeginRenderPass(const Framebuffer* framebuffer, bool explicitClear)
	{
//		ETH_PROFILE_FUNCTION(fmt::format("VulkanRenderer::BeginRenderPass ({})", renderPass->GetSpecification().DebugName).c_str());

		uint32_t frameIndex = m_Context->GetSwapchain()->GetCurrentFrameIndex();
		VkCommandBuffer commandBuffer = m_RenderCommandBuffer->GetCommandBuffer(frameIndex)->GetHandle();

        const VulkanFramebuffer* fb = dynamic_cast<const VulkanFramebuffer*>(framebuffer);
		const auto& fbSpec = fb->GetSpecification();

		uint32_t width, height;

		VkViewport viewport = {};
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		if (framebuffer->GetSpecification().SwapChainTarget)
		{
			const VulkanSwapChain* swapChain = m_Context->GetSwapchain();
			width = swapChain->GetWidth();
			height = swapChain->GetHeight();
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.pNext = nullptr;
			renderPassBeginInfo.renderPass = fb->GetRenderPass()->GetHandle();
			renderPassBeginInfo.renderArea.offset.x = 0;
			renderPassBeginInfo.renderArea.offset.y = 0;
			renderPassBeginInfo.renderArea.extent.width = width;
			renderPassBeginInfo.renderArea.extent.height = height;
			renderPassBeginInfo.framebuffer = swapChain->GetCurrentFramebuffer();

			viewport.x = 0.0f;
			viewport.y = (float)height;
			viewport.width = (float)width;
			viewport.height = -(float)height;
		}
		else
		{
			width = fb->GetWidth();
			height = fb->GetHeight();
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.pNext = nullptr;
			renderPassBeginInfo.renderPass = fb->GetRenderPass()->GetHandle();
			renderPassBeginInfo.renderArea.offset.x = 0;
			renderPassBeginInfo.renderArea.offset.y = 0;
			renderPassBeginInfo.renderArea.extent.width = width;
			renderPassBeginInfo.renderArea.extent.height = height;
			renderPassBeginInfo.framebuffer = fb->GetVulkanFramebuffer();

			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)width;
			viewport.height = (float)height;
		}

		// TODO: Does our framebuffer have a depth attachment?
		const auto& clearValues = fb->GetVulkanClearValues();
		renderPassBeginInfo.clearValueCount = (uint32_t)clearValues.size();
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		if (explicitClear)
		{
			const uint32_t colorAttachmentCount = (uint32_t)fb->GetColorAttachmentCount();
			const uint32_t totalAttachmentCount = colorAttachmentCount + (fb->HasDepthAttachment() ? 1 : 0);
			ETH_CORE_ASSERT(clearValues.size() == totalAttachmentCount);

			std::vector<VkClearAttachment> attachments(totalAttachmentCount);
			std::vector<VkClearRect> clearRects(totalAttachmentCount);
			for (uint32_t i = 0; i < colorAttachmentCount; i++)
			{
				attachments[i].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				attachments[i].colorAttachment = i;
				attachments[i].clearValue = clearValues[i];

				clearRects[i].rect.offset = { (int32_t)0, (int32_t)0 };
				clearRects[i].rect.extent = { width, height };
				clearRects[i].baseArrayLayer = 0;
				clearRects[i].layerCount = 1;
			}

			if (fb->HasDepthAttachment())
			{
				attachments[colorAttachmentCount].aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
				attachments[colorAttachmentCount].clearValue = clearValues[colorAttachmentCount];
				clearRects[colorAttachmentCount].rect.offset = { (int32_t)0, (int32_t)0 };
				clearRects[colorAttachmentCount].rect.extent = { width, height };
				clearRects[colorAttachmentCount].baseArrayLayer = 0;
				clearRects[colorAttachmentCount].layerCount = 1;
			}

			vkCmdClearAttachments(commandBuffer, totalAttachmentCount, attachments.data(), totalAttachmentCount, clearRects.data());

		}

		// Update dynamic viewport state
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		// Update dynamic scissor state
		VkRect2D scissor = {};
		scissor.extent.width = width;
		scissor.extent.height = height;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void VulkanRendererAPI::EndRenderPass()
	{
//		ETH_PROFILE_FUNCTION("VulkanRenderer::EndRenderPass");

		uint32_t frameIndex = m_Context->GetSwapchain()->GetCurrentFrameIndex();
		VkCommandBuffer commandBuffer = m_RenderCommandBuffer->GetCommandBuffer(frameIndex)->GetHandle();

		vkCmdEndRenderPass(commandBuffer);
	}
}
