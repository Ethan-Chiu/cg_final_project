#pragma once

#include "RendererAPI.h"

#include "Image.h"
#include "RenderTarget.h"

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
        
        static void BeginRenderTarget(const RenderTarget* target, bool explicitClear = false);
        static void EndRenderTarget();
        
        static const GraphicsContext* GetGraphicsContext(uint32_t i) { return s_ContextList[i]; }
        
        static TargetImage* GetSwapchainTarget();
        
        static const RendererConfig& GetRendererConfig() { return s_Config; };

	private:
		static RendererConfig s_Config;
        static Scope<RendererAPI> s_RendererAPI;
        static std::vector<const GraphicsContext*> s_ContextList;
	};

}
