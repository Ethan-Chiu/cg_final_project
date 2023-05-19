#pragma once

#include "RendererAPI.h"

namespace Ethane {
	
	struct RendererConfig
	{
		uint32_t FramesInFlight = 3;
	};

	class Renderer
	{
	public:
		static void Init(const GraphicsContext* ctx);
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginFrame();
		static void EndFrame();

	private:
		static RendererConfig s_Config;
        static Scope<RendererAPI> s_RendererAPI;
	};

}
