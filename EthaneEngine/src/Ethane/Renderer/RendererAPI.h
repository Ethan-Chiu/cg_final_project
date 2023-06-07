#pragma once

#include <glm/glm.hpp>

namespace Ethane {

    struct RendererConfig;
    class GraphicsContext;
    class TargetImage;
    class RenderTarget;
    class VertexBuffer;
    class IndexBuffer;
    class Mesh;
    class Material;
    class Pipeline;
    class ComputePipeline;
    class Shader;
    class RenderCommandBuffer;
    enum class ImageLayout;
    enum class AccessMask;
    enum class PipelineStage;

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, Vulkan = 1
		};
        
    public:
        virtual void Init(const RendererConfig& config, const GraphicsContext* ctx) = 0;
        virtual void Shutdown() = 0;
        
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
        virtual void SetClearColor(const glm::vec4& color) = 0;
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        
        virtual void RegisterShader(const Shader* shader) = 0;
        virtual void SetGlobalUniformBuffer(uint32_t binding, const void* data, uint32_t size) = 0;
        
        virtual void BeginRenderTarget(const RenderTarget* target, bool explicitClear = false) = 0;
        virtual void EndRenderTarget() = 0;
        
        virtual void DrawGeometry(Ref<Pipeline> pipeline, Ref<VertexBuffer> vertexbuffer, Ref<IndexBuffer> indexbuffer, Ref<Material> material) = 0;
        virtual void DrawFullscreenQuad(Ref<Pipeline> pipeline, Ref<Material> material) = 0;
        virtual void DrawMesh(Ref<Pipeline> pipeline, Mesh* mesh, Material* material, const glm::mat4& transform) = 0;
        
        virtual void TransitionLayout(TargetImage* targetImage, ImageLayout oldLayout, ImageLayout newLayout, AccessMask srcAccessMask, PipelineStage srcStage, AccessMask dstAccessMask, PipelineStage dstStage, Ref<RenderCommandBuffer> renderCmdBuffer) = 0;
        virtual void BeginCompute(Ref<ComputePipeline> computePipeline, Ref<Material> material, uint32_t worker_x, uint32_t worker_y, uint32_t worker_z, Ref<RenderCommandBuffer> renderCmdBuffer) = 0;
        
        virtual TargetImage* GetSwapchainTarget() const = 0;
        
    public:
        virtual ~RendererAPI() = default;

        inline static API GetAPI() { return s_API; }
        inline static void SetAPI(API _API) { s_API = _API; }
        
	private:
		static API s_API;
	};

}
