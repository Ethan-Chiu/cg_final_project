#pragma once

#include "RendererAPI.h"

#include "Image.h"
#include "RenderTarget.h"
#include "Pipeline.h"
#include "Mesh.h"

namespace Ethane {
	
	struct RendererConfig
	{
        uint32_t DefaultWindowWidth;
        uint32_t DefaultWindowHeight;
	};

	class Renderer
	{
	public:
		static void Init(const GraphicsContext* ctx, const RendererConfig& config);
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginFrame();
		static void EndFrame();
        
        static void RegisterShader(const Shader* shader);
        
        static void BeginRenderTarget(const RenderTarget* target, bool explicitClear = false);
        static void EndRenderTarget();
        
        static void RegisterShader();
        static void AquireMaterialResource();
        static void SetGlobalUniformBuffer(uint32_t binding, const void* data, uint32_t size);
        
        static void DrawMesh(Ref<Pipeline> pipeline, Ref<Mesh> mesh, Ref<Material> material, const glm::mat4& transform);
        
        static const GraphicsContext* GetGraphicsContext() { return s_Context; }
        
        static TargetImage* GetSwapchainTarget();
        
        static const RendererConfig& GetRendererConfig() { return s_Config; };

	private:
		static RendererConfig s_Config;
        static Scope<RendererAPI> s_RendererAPI;
        static const GraphicsContext* s_Context;
	};

}
