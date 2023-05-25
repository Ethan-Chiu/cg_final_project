#include "ethpch.h"
#include "Renderer.h"
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanRendererAPI.h"

namespace Ethane {

	RendererConfig Renderer::s_Config = RendererConfig{};
    Scope<RendererAPI> Renderer::s_RendererAPI = nullptr;

	void Renderer::Init(const GraphicsContext* ctx, const RendererConfig& config)
	{
        s_Config = config;
		ETH_PROFILE_FUNCTION();
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None:
            ETH_CORE_ASSERT("endererAPI::None is currently not supported!");
            break;
        case RendererAPI::API::Vulkan:
            s_RendererAPI = CreateScope<VulkanRendererAPI>();
            ETH_CORE_INFO("Vulkan graphic API selected");
            break;
        }
        s_RendererAPI->Init(s_Config, ctx);
	}

	void Renderer::Shutdown()
	{
        s_RendererAPI->Shutdown();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		s_RendererAPI->SetViewport(0, 0, width, height);
	}

	void Renderer::BeginFrame()
	{
        s_RendererAPI->BeginFrame();
	}

	void Renderer::EndFrame()
	{
        s_RendererAPI->EndFrame();
	}

    void Renderer::RegisterShader(const Shader* shader)
    {
        s_RendererAPI->RegisterShader(shader);
    }

    void Renderer::SetGlobalUniformBuffer(uint32_t binding, const void* data, uint32_t size)
    {
        s_RendererAPI->SetGlobalUniformBuffer(binding, data, size);
    }

    void Renderer::BeginRenderTarget(const RenderTarget* target, bool explicitClear)
    {
        s_RendererAPI->BeginRenderTarget(target, explicitClear);
    }

    void Renderer::EndRenderTarget()
    {
        s_RendererAPI->EndRenderTarget();
    }

    void Renderer::DrawGeometry(Ref<Pipeline> pipeline, Ref<VertexBuffer> vertexbuffer, Ref<IndexBuffer> indexbuffer, Ref<Material> material)
    {
        s_RendererAPI->DrawGeometry(pipeline, vertexbuffer, indexbuffer, material);
    }

    void Renderer::DrawFullscreenQuad(Ref<Pipeline> pipeline, Ref<Material> material)
    {
        s_RendererAPI->DrawFullscreenQuad(pipeline, material);
    }

    void Renderer::DrawMesh(Ref<Pipeline> pipeline, Ref<Mesh> mesh, Ref<Material> material, const glm::mat4& transform)
    {
        s_RendererAPI->DrawMesh(pipeline, mesh, material, transform);
    }

    TargetImage* Renderer::GetSwapchainTarget()
    {
        return s_RendererAPI->GetSwapchainTarget();
    }
	
}
