#pragma once

#include "RendererAPI.h"

#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanRenderCommandBuffer.h"
#include "VulkanRenderTarget.h"
#include "VulkanMaterial.h"

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

			std::set<VulkanMaterial*> UpdatedMaterial = {};
		};
        
	public:
		VulkanRendererAPI() = default;
		~VulkanRendererAPI() = default;

		virtual void Init(const RendererConfig& config, const GraphicsContext* ctx) override;
        virtual void Shutdown() override;

		void BeginFrame() override;
		void EndFrame() override;
        
        virtual VulkanTargetImage* GetSwapchainTarget() const override;

        void RegisterShader(const Shader* shader) override;
        void SetGlobalUniformBuffer(uint32_t binding, const void* data, uint32_t size) override;
        
        // TODO:
        void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override {};
        void SetClearColor(const glm::vec4& color) override {};
        
		void BeginRenderCommandBuffer();
		void EndRenderCommandBuffer();

		virtual void BeginRenderTarget(const RenderTarget* target, bool explicitClear = false) override;
        virtual void EndRenderTarget() override;
	private:
		void UpdateMaterialForRendering(VulkanMaterial* material);
		void CmdBindMaterial(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, const VulkanMaterial* material, uint32_t frameIndex);

	public:
//		static VkDescriptorSet AllocateDescriptorSet(VkDescriptorSetAllocateInfo& allocInfo);

		// Draw
//		virtual void DrawIndexed(uint32_t indexCount = 0) override {}; // TODO: remove
//		virtual void DrawMesh(Ref<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f)) override {}; // TODO: remove
//
//		static void DrawQuad(Ref<Pipeline> pipeline, Ref<Material> material, const glm::mat4& transform) ; // Ref<StorageBufferSet> storageBufferSet
//
        void DrawGeometry(Ref<Pipeline> pipeline, Ref<VertexBuffer> vertexbuffer, Ref<IndexBuffer> indexbuffer, Ref<Material> material);
        
        void DrawFullscreenQuad(Ref<Pipeline> pipeline, Ref<Material> material);
        
		void DrawMesh(Ref<Pipeline> pipeline, Ref<Mesh> mesh, Ref<Material> material, const glm::mat4& transform) override;
        
//
        // virtual void DrawMesh(Ref<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f)) override;
        
//		// Update uniform buffer value
//		static void SetUniformBuffer(uint32_t binding, uint32_t set, const void* data, uint32_t size, uint32_t offset = 0);
	private:
        Scope<VulkanRenderCommandBuffer> m_RenderCommandBuffer = nullptr;

		static VulkanRendererData* s_Data;
	};

}
