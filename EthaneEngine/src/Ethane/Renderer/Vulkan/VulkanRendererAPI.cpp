#include "ethpch.h"
#include "VulkanRendererAPI.h"
#include "VulkanContext.h"
#include "VulkanSwapChain.h"

#include "VulkanPipeline.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"

#include "./ShaderUtils/VulkanShaderSystem.h"

#include "Ethane/Renderer/Mesh.h"

namespace Ethane {

	VulkanRendererAPI::VulkanRendererData* VulkanRendererAPI::s_Data = nullptr;

	void VulkanRendererAPI::Init(const RendererConfig& config, const GraphicsContext* ctx)
	{
		s_Data = new VulkanRendererData();

        auto imageCount = VulkanContext::GetSwapchain()->GetImageCount();
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
		VkDevice device = VulkanContext::GetDevice()->GetVulkanDevice();
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

		s_Data->QuadVertexBuffer = CreateRef<VulkanVertexBuffer>(data, 4 * sizeof(QuadVertex));
		uint32_t indices[6] = { 0, 1, 2, 2, 3, 0, };
		s_Data->QuadIndexBuffer = CreateRef<VulkanIndexBuffer>(indices, 6 * sizeof(uint32_t));
        
        m_RenderCommandBuffer = CreateScope<VulkanRenderCommandBuffer>();
	}

    void VulkanRendererAPI::Shutdown()
    {
        m_RenderCommandBuffer->Destroy();
        
        s_Data->QuadVertexBuffer->Destroy();
        s_Data->QuadIndexBuffer->Destroy();
        
        for (auto pool : s_Data->DescriptorPools)
        {
            vkDestroyDescriptorPool(VulkanContext::GetDevice()->GetVulkanDevice(), pool, nullptr);
        }
        s_Data->DescriptorPools.clear();
        
        delete s_Data;
        
        VulkanShaderSystem::Shutdown();
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
        BeginRenderCommandBuffer();
        s_Data->UpdatedMaterial.clear();
	}

    void VulkanRendererAPI::EndFrame()
    {
        EndRenderCommandBuffer();
        m_RenderCommandBuffer->Submit();
    }

    VulkanTargetImage* VulkanRendererAPI::GetSwapchainTarget() const
    {
        return VulkanContext::GetSwapchain()->GetTargetImage();
    }

	void VulkanRendererAPI::BeginRenderCommandBuffer()
	{
        m_RenderCommandBuffer->Begin();
	}

	void VulkanRendererAPI::EndRenderCommandBuffer()
	{
        m_RenderCommandBuffer->End();
	}

    void VulkanRendererAPI::RegisterShader(const Shader* shader)
    {
        VulkanShaderSystem::RegisterShader(static_cast<const VulkanShader*>(shader));
    }

    void VulkanRendererAPI::SetGlobalUniformBuffer(uint32_t binding, const void* data, uint32_t size)
    {
        VulkanShaderSystem::SetGlobalUniformBuffer(binding, data, size);
    }

    void VulkanRendererAPI::UpdateMaterialForRendering(VulkanMaterial* material)
    {
        ETH_PROFILE_FUNCTION();
        if (s_Data->UpdatedMaterial.find(material) == s_Data->UpdatedMaterial.end())
        {
            material->ApplyMaterial();
            s_Data->UpdatedMaterial.insert(material);
        }
    }

    void VulkanRendererAPI::CmdBindMaterial(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, const VulkanMaterial* material, uint32_t frameIndex)
    {
        ETH_PROFILE_FUNCTION();
        std::vector<VkDescriptorSet> descriptorSet = material->GetDescriptorSets(frameIndex);
        if (!descriptorSet.empty())
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, (uint32_t)(descriptorSet.size()), descriptorSet.data(), 0, nullptr);
    }

	void VulkanRendererAPI::BeginRenderTarget(const RenderTarget* render_target, bool explicitClear)
	{
//		ETH_PROFILE_FUNCTION(fmt::format("VulkanRenderer::BeginRenderPass ({})", renderPass->GetSpecification().DebugName).c_str());
        const VulkanRenderTarget* target = static_cast<const VulkanRenderTarget*>(render_target);

		uint32_t frameIndex = VulkanContext::GetSwapchain()->GetCurrentFrameIndex();
//		VkCommandBuffer commandBuffer = m_RenderCommandBuffer->GetCommandBuffer(frameIndex)->GetHandle();
        //TODO: 
        VkCommandBuffer commandBuffer = VulkanContext::GetSwapchain()->GetCurrentCommandBuffer()->GetHandle();

		uint32_t width, height;

		VkViewport viewport = {};
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		if (target->GetSpecification().SwapChainTarget)
		{
			const VulkanSwapChain* swapChain = VulkanContext::GetSwapchain();
			width = swapChain->GetWidth();
			height = swapChain->GetHeight();
			renderPassBeginInfo.pNext = nullptr;
			renderPassBeginInfo.renderPass = target->GetRenderPass()->GetHandle();
			renderPassBeginInfo.renderArea.offset.x = 0;
			renderPassBeginInfo.renderArea.offset.y = 0;
			renderPassBeginInfo.renderArea.extent.width = width;
			renderPassBeginInfo.renderArea.extent.height = height;
			renderPassBeginInfo.framebuffer = target->GetCurrentFramebuffer()->GetHandle();

			viewport.x = 0.0f;
			viewport.y = (float)height;
			viewport.width = (float)width;
			viewport.height = -(float)height;
		}
		else
		{
			width = target->GetWidth();
			height = target->GetHeight();
			renderPassBeginInfo.pNext = nullptr;
			renderPassBeginInfo.renderPass = target->GetRenderPass()->GetHandle();
			renderPassBeginInfo.renderArea.offset.x = 0;
			renderPassBeginInfo.renderArea.offset.y = 0;
			renderPassBeginInfo.renderArea.extent.width = width;
			renderPassBeginInfo.renderArea.extent.height = height;
			renderPassBeginInfo.framebuffer = target->GetCurrentFramebuffer()->GetHandle();

			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)width;
			viewport.height = (float)height;
		}

		const auto& clearValues = target->GetVulkanClearValues();
		renderPassBeginInfo.clearValueCount = (uint32_t)clearValues.size();
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		if (explicitClear)
		{
			const uint32_t colorAttachmentCount = (uint32_t)target->GetColorAttachmentCount();
			const uint32_t totalAttachmentCount = colorAttachmentCount + (target->HasDepthAttachment() ? 1 : 0);
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

			if (target->HasDepthAttachment())
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

	void VulkanRendererAPI::EndRenderTarget()
	{
        ETH_PROFILE_SCOPE("VulkanRenderer::EndRenderPass");

//		uint32_t frameIndex = m_Context->GetSwapchain()->GetCurrentFrameIndex();
        //TODO:
//		VkCommandBuffer commandBuffer = m_RenderCommandBuffer->GetCommandBuffer(frameIndex)->GetHandle();
        VkCommandBuffer commandBuffer = VulkanContext::GetSwapchain()->GetCurrentCommandBuffer()->GetHandle();
        
		vkCmdEndRenderPass(commandBuffer);
	}

    void VulkanRendererAPI::DrawMesh(Ref<Pipeline> pipeline, Ref<Mesh> mesh, Ref<Material> material, const glm::mat4& transform)
    {
        ETH_CORE_ASSERT(mesh);
        
        uint32_t frameIndex = VulkanContext::GetSwapchain()->GetCurrentFrameIndex();
        VkCommandBuffer commandBuffer = VulkanContext::GetSwapchain()->GetCurrentCommandBuffer()->GetHandle();
//        VkCommandBuffer commandBuffer = std::dynamic_pointer_cast<VulkanRenderCommandBuffer>(s_RenderCommandBuffer)->GetCommandBuffer(frameIndex);

        Ref<VulkanMaterial> vulkanMaterial = std::dynamic_pointer_cast<VulkanMaterial>(material);

        auto vulkanMeshVB = std::dynamic_pointer_cast<VulkanVertexBuffer>(mesh->m_VertexBuffer);
        VkBuffer vbMeshBuffer = vulkanMeshVB->GetVulkanBuffer();
        VkDeviceSize offsets[1] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vbMeshBuffer, offsets);

        auto vulkanMeshIB = std::dynamic_pointer_cast<VulkanIndexBuffer>(mesh->m_IndexBuffer);// Ref<VulkanIndexBuffer>(meshAsset->GetIndexBuffer());
        VkBuffer ibBuffer = vulkanMeshIB->GetVulkanBuffer();
        vkCmdBindIndexBuffer(commandBuffer, ibBuffer, 0, VK_INDEX_TYPE_UINT32);

        Ref<VulkanPipeline> vulkanPipeline = std::dynamic_pointer_cast<VulkanPipeline>(pipeline);
        VkPipelineLayout layout = vulkanPipeline->GetPipelineLayout();
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetVulkanPipeline());

        float lineWidth = 1.0;// vulkanPipeline->GetSpecification().LineWidth;
        if (lineWidth != 1.0f)
            vkCmdSetLineWidth(commandBuffer, lineWidth);

        UpdateMaterialForRendering(vulkanMaterial.get());
        CmdBindMaterial(commandBuffer, layout, vulkanMaterial.get(), frameIndex);

        auto& submeshes = mesh->m_Submeshes;
        for (auto& submesh : submeshes)
        {
            glm::mat4 worldTransform = transform * submesh.Transform;
            vkCmdPushConstants(commandBuffer, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &worldTransform);
            vkCmdDrawIndexed(commandBuffer, submesh.IndexCount, 1, submesh.BaseIndex, submesh.BaseVertex, 0);
        }
    }

    void VulkanRendererAPI::DrawGeometry(Ref<Pipeline> pipeline, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, Ref<Material> material)
    {
        Ref<VulkanMaterial> vulkanMaterial = std::dynamic_pointer_cast<VulkanMaterial>(material);

        uint32_t frameIndex = VulkanContext::GetSwapchain()->GetCurrentFrameIndex();
        VkCommandBuffer commandBuffer = VulkanContext::GetSwapchain()->GetCurrentCommandBuffer()->GetHandle();
//        VkCommandBuffer commandBuffer = std::dynamic_pointer_cast<VulkanRenderCommandBuffer>(s_RenderCommandBuffer)->GetCommandBuffer(frameIndex);

        auto vulkanMeshVB = std::dynamic_pointer_cast<VulkanVertexBuffer>(vertexBuffer);
        VkBuffer vbMeshBuffer = vulkanMeshVB->GetVulkanBuffer();
        VkDeviceSize offsets[1] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vbMeshBuffer, offsets);

        auto vulkanMeshIB = std::dynamic_pointer_cast<VulkanIndexBuffer>(indexBuffer);
        VkBuffer ibBuffer = vulkanMeshIB->GetVulkanBuffer();
        vkCmdBindIndexBuffer(commandBuffer, ibBuffer, 0, VK_INDEX_TYPE_UINT32);

        Ref<VulkanPipeline> vulkanPipeline = std::dynamic_pointer_cast<VulkanPipeline>(pipeline);
        VkPipelineLayout layout = vulkanPipeline->GetPipelineLayout();
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetVulkanPipeline());

        UpdateMaterialForRendering(vulkanMaterial.get());
        CmdBindMaterial(commandBuffer, layout, vulkanMaterial.get(), frameIndex);

        vkCmdDrawIndexed(commandBuffer, indexBuffer->GetCount(), 1, 0, 0, 0);
    }

    void VulkanRendererAPI::DrawFullscreenQuad(Ref<Pipeline> pipeline, Ref<Material> material)
    {
        DrawGeometry(pipeline, s_Data->QuadVertexBuffer, s_Data->QuadIndexBuffer, material);
    }

}
