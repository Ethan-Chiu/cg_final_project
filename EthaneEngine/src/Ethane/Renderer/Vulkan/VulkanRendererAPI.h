#pragma once

#include "RendererAPI.h"

#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanRenderCommandBuffer.h"
#include "VulkanFramebuffer.h"

namespace Ethane {

	class VulkanRendererAPI : public RendererAPI
	{
	public:
		struct VulkanRendererData
		{
			Ref<VulkanVertexBuffer> QuadVertexBuffer = nullptr;
			Ref<VulkanIndexBuffer> QuadIndexBuffer = nullptr;
//			VulkanShader::DescriptorSetsAndPool QuadDescriptorSet;

			VkDescriptorSet ActiveRendererDescriptorSet = nullptr;
			std::vector<VkDescriptorPool> DescriptorPools;

            // stats
            std::vector<uint32_t> DescriptorPoolAllocationCount;

			// Default samplers
			VkSampler SamplerClamp = nullptr;

//			std::set<Ref<VulkanMaterial>> UpdatedMaterial = {};
		};
        
	public:
		VulkanRendererAPI() = default;
		~VulkanRendererAPI() = default;

		virtual void Init(const RendererConfig& config, const GraphicsContext* ctx) override;
        virtual void Shutdown() override;

		void BeginFrame() override;
		void EndFrame() override {};

        // TODO:
        void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override {};
        void SetClearColor(const glm::vec4& color) override {};
        
		void BeginRenderCommandBuffer();
		void EndRenderCommandBuffer();

		void BeginRenderPass(const Framebuffer* framebuffer, bool explicitClear = false);
		void EndRenderPass();
	private:
//		static void UpdateMaterialForRendering(Ref<VulkanMaterial> material);
//		static void CmdBindMaterial(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, Ref<VulkanMaterial> material, uint32_t frameIndex);

	public:
//		static VkDescriptorSet AllocateDescriptorSet(VkDescriptorSetAllocateInfo& allocInfo);

		// Draw
//		virtual void DrawIndexed(uint32_t indexCount = 0) override {}; // TODO: remove
//		virtual void DrawMesh(Ref<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f)) override {}; // TODO: remove
//
//		static void DrawQuad(Ref<Pipeline> pipeline, Ref<Material> material, const glm::mat4& transform) ; // Ref<StorageBufferSet> storageBufferSet
//
//		static void DrawFullscreenQuad(Ref<Pipeline> pipeline, Ref<Material> material); // Ref<RenderCommandBuffer> renderCommandBuffer,
//		// void SubmitFullscreenQuad(Ref<RenderCommandBuffer> renderCommandBuffer, Ref<Pipeline> pipeline, Ref<UniformBufferSet> uniformBufferSet, Ref<StorageBufferSet> storageBufferSet, Ref<Material> material) override;
//
//		static void DrawMesh(Ref<Pipeline> pipeline, Ref<Mesh> mesh, Ref<Material> material, const glm::mat4& transform); // , Ref<MaterialTable> materialTable
//		// virtual void DrawMesh(Ref<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f)) override;
//
//		static void DrawGeometry(Ref<Pipeline> pipeline, Ref<VertexBuffer> vertexbuffer, Ref<IndexBuffer> indexbuffer, Ref<Material> material, const glm::mat4& transform = glm::mat4(1.0f), uint32_t indexCount = 0);
//
//		// Update uniform buffer value
//		static void SetUniformBuffer(uint32_t binding, uint32_t set, const void* data, uint32_t size, uint32_t offset = 0);
	private:
        const VulkanContext* m_Context = nullptr;
        Scope<VulkanRenderCommandBuffer> m_RenderCommandBuffer = nullptr;

		static VulkanRendererData* s_Data;
	};

}
