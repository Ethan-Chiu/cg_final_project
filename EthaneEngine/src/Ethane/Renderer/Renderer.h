#pragma once

#include "RendererAPI.h"

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
        
        static const RendererConfig& GetRendererConfig() { return s_Config; };

	private:
		static RendererConfig s_Config;
        static Scope<RendererAPI> s_RendererAPI;
	};

}
