#pragma once

#include <glm/glm.hpp>

namespace Ethane {

    struct RendererConfig;
    class GraphicsContext;
    class TargetImage;
    class RenderTarget;

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
        
        virtual void BeginRenderTarget(const RenderTarget* target, bool explicitClear = false) = 0;
        virtual void EndRenderTarget() = 0;
        
        virtual TargetImage* GetSwapchainTarget() const = 0;
        
    public:
        virtual ~RendererAPI() = default;

        inline static API GetAPI() { return s_API; }
        inline static void SetAPI(API _API) { s_API = _API; }
        
	private:
		static API s_API;
	};

}
