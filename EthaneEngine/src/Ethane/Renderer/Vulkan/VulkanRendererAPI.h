#pragma once

#include "RendererAPI.h"

#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanRenderCommandBuffer.h"
#include "VulkanRenderTarget.h"
#include "VulkanMaterial.h"
#include "VulkanRenderCommandBuffer.h"

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
		void CmdBindMaterial(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, const VulkanMaterial* material, uint32_t frameIndex, VkPipelineBindPoint bindpoint = VK_PIPELINE_BIND_POINT_GRAPHICS);

	public:
//		static VkDescriptorSet AllocateDescriptorSet(VkDescriptorSetAllocateInfo& allocInfo);

		// Draw
//		virtual void DrawIndexed(uint32_t indexCount = 0) override {}; // TODO: remove
//		virtual void DrawMesh(Ref<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f)) override {}; // TODO: remove
//
//		static void DrawQuad(Ref<Pipeline> pipeline, Ref<Material> material, const glm::mat4& transform) ; // Ref<StorageBufferSet> storageBufferSet
//
        void DrawGeometry(Ref<Pipeline> pipeline, Ref<VertexBuffer> vertexbuffer, Ref<IndexBuffer> indexbuffer, Ref<Material> material) override;
        
        void DrawFullscreenQuad(Ref<Pipeline> pipeline, Ref<Material> material) override;
        
		void DrawMesh(Ref<Pipeline> pipeline, Mesh* mesh, Material* material, const glm::mat4& transform) override;
        
        void TransitionLayout(TargetImage* targetImage, ImageLayout oldLayout, ImageLayout newLayout, AccessMask srcAccessMask, PipelineStage srcStage, AccessMask dstAccessMask, PipelineStage dstStage, Ref<RenderCommandBuffer> renderCmdBuffer) override;
        void BeginCompute(Ref<ComputePipeline> computePipeline, Ref<Material> material, uint32_t worker_x, uint32_t worker_y, uint32_t worker_z, Ref<RenderCommandBuffer> renderCmdBuffer) override;
        
	private:
//        Scope<VulkanRenderCommandBuffer> m_RenderCommandBuffer = nullptr;

		static VulkanRendererData* s_Data;
	};

}
