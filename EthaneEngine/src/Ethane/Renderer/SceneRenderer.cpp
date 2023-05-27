#include "ethpch.h"
#include "SceneRenderer.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Ethane/Systems/ShaderSystem.h"


namespace Ethane {

	SceneRenderer::SceneRenderer(Ref<Scene> scene)
		: m_Scene(scene)
	{
		Init();
	}


	void SceneRenderer::Init()
	{
        RendererConfig rendererConfig = Renderer::GetRendererConfig();
        
        ImageSpecification imageSpec = {};
        imageSpec.Usage = ImageUsage::Attachment;
        imageSpec.Width = rendererConfig.DefaultWindowWidth;
        imageSpec.Height = rendererConfig.DefaultWindowHeight;
        
		// Geometry
		{
            m_GeoColor = Renderer::GetSwapchainTarget();
            imageSpec.DebugName = "GeoDepth";
            imageSpec.Format = ImageFormat::DEPTH32F;
            m_GeoDepth = Image2D::Create(imageSpec);

			RenderTargetSpecification geoTargetSpec;
            geoTargetSpec.Attachments = { m_GeoColor, m_GeoDepth.get() };
            geoTargetSpec.IsTargetImage = true;
            geoTargetSpec.ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
            geoTargetSpec.DebugName = "Geometry";
            geoTargetSpec.Width = rendererConfig.DefaultWindowWidth;
            geoTargetSpec.Height = rendererConfig.DefaultWindowHeight;
            geoTargetSpec.SwapChainTarget = true;
			m_GeoTarget = RenderTarget::Create(geoTargetSpec);

			PipelineSpecification pipelineSpecification;
			pipelineSpecification.Layout = {
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float3, "a_Normal" },
				{ ShaderDataType::Float3, "a_Tangent" },
				{ ShaderDataType::Float3, "a_Binormal" },
				{ ShaderDataType::Float2, "a_TexCoord" },
			};
            auto shader = ShaderSystem::Get("test");
            pipelineSpecification.Shader = shader;
            pipelineSpecification.RenderPass = m_GeoTarget->GetRenderPass();
			m_GeometryPipeline = Pipeline::Create(pipelineSpecification);
		}

        {
            ComputePipelineSpecification pipelineSpec;
            auto shader = ShaderSystem::Get("test_compute");
            m_ComputeMat = Material::Create(shader.get());
            m_ComputeMat->SetImage("colorBuffer", m_GeoColor);
            pipelineSpec.Shader = shader;
            m_MorphingPipeline = ComputePipeline::Create(pipelineSpec);
        }
        
		// Grid
//		{
//			m_GridShader = ShaderLibrary::Get("Grid");
//			const float gridScale = 16.025f;
//			const float gridSize = 0.025f;
//			m_GridMaterial = Material::Create(m_GridShader);
//
//			PipelineSpecification pipelineSpec;
//			pipelineSpec.Shader = m_GridShader;
//			pipelineSpec.Layout = {
//				{ ShaderDataType::Float3, "a_Position" },
//				{ ShaderDataType::Float2, "a_TexCoord" }
//			};
//			pipelineSpec.RenderPass = m_GeometryPipeline->GetSpecification().RenderPass;
//			m_GridPipeline = Pipeline::Create(pipelineSpec);
//		}

		// Composite
		{
//			FramebufferSpecification compFramebufferSpec;
//			compFramebufferSpec.ClearColor = { 0.5f, 0.1f, 0.1f, 1.0f };
//			compFramebufferSpec.DebugName = "SceneComposite";
//			// TODO: width and height
//			compFramebufferSpec.Width = 1600;
//			compFramebufferSpec.Height = 900;
//
//			compFramebufferSpec.Attachments = { ImageFormat::RGBA };
//
//			Ref<Framebuffer> framebuffer = Framebuffer::Create(compFramebufferSpec);
//
//			RenderPassSpecification renderPassSpec;
//			renderPassSpec.TargetFramebuffer = framebuffer;
//			// renderPassSpec.DebugName = "SceneComposite";
//
//			PipelineSpecification pipelineSpecification;
//			pipelineSpecification.Layout = {
//				{ ShaderDataType::Float3, "a_Position" },
//				{ ShaderDataType::Float2, "a_TexCoord" }
//			};
//			// pipelineSpecification.BackfaceCulling = false;
//			pipelineSpecification.Shader = ShaderLibrary::Get("SceneComposite");
//			pipelineSpecification.RenderPass = RenderPass::Create(renderPassSpec);
//			m_CompositePipeline = Pipeline::Create(pipelineSpecification);
//
//			// TODO: test
//			if (m_Texture2D == nullptr)
//				m_Texture2D = Texture2D::Create("assets/textures/test.png");
//			m_CompositeMaterial = Material::Create(ShaderLibrary::Get("SceneComposite"), "Composite material");
//
//			auto geoFramebuffer = m_GeometryPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer;
//			m_CompositeMaterial->Set("u_Texture", std::dynamic_pointer_cast<VulkanFramebuffer>(geoFramebuffer)->GetTexture());
		}
	}

	void SceneRenderer::SetViewportSize(uint32_t width, uint32_t height)
	{
		if (m_ViewportWidth != width || m_ViewportHeight != height)
		{
			m_ViewportWidth = width;
			m_ViewportHeight = height;
			m_InvViewportWidth = 1.f / (float)width;
			m_InvViewportHeight = 1.f / (float)height;
			m_NeedResize = true;
		}
	}

	void SceneRenderer::BeginScene(const Camera& camera, const glm::mat4& viewMatrix)
	{
		ETH_PROFILE_FUNCTION();

		if (m_NeedResize)
		{
			m_NeedResize = false;
		
            m_GeoTarget->Resize(m_ViewportWidth, m_ViewportHeight);
            
            m_GeoColor = Renderer::GetSwapchainTarget();
            m_ComputeMat->SetImage("colorBuffer", m_GeoColor);
//			m_GeoFramebuffer->Resize(m_ViewportWidth, m_ViewportHeight);
//			->Resize(m_ViewportWidth, m_ViewportHeight);

//			m_CompositeMaterial->Set("u_Texture", std::dynamic_pointer_cast<VulkanFramebuffer>(geoFramebuffer)->GetTexture());
		}


		// Update uniform buffers
		UBCamera& cameraData = CameraDataUB;

		auto& sceneCamera = camera;
		const auto viewProjection = sceneCamera.GetProjection() * viewMatrix;
		const glm::vec3 cameraPosition = glm::inverse(viewMatrix)[3];
		const auto inverseVP = glm::inverse(viewProjection);

		cameraData.ViewProjection = viewProjection;
		cameraData.InverseViewProjection = inverseVP;
		cameraData.Projection = sceneCamera.GetProjection();
		cameraData.View = viewMatrix;

		UBGlobal& globalData = m_GlobalUB;
		globalData.ViewProjection = viewProjection;
		globalData.AmbientColor = glm::vec4(0.8, 0.8, 0.8, 1);
		globalData.ViewPosition = cameraPosition;
		
		UBLocal localUB;
		localUB.DiffuseColor = glm::vec4(1, 1, 1, 1);
		localUB.Shininess = 32;

        Renderer::SetGlobalUniformBuffer(0, (void*)&(cameraData.ViewProjection), sizeof(glm::mat4));
	}

	void SceneRenderer::SubmitMesh(Ref<Mesh> mesh, Ref<Material> material, const glm::mat4& transform)
	{
		// TODO: Culling, sorting, etc.
		m_DrawList.push_back({ mesh, transform, material});
	}

	void SceneRenderer::SubmitSelectedMesh(Ref<Mesh> mesh, const glm::mat4& transform) //, Ref<Material> material
	{
		m_SelectedMeshDrawList.push_back({ mesh, transform }); // , material
	}

	void SceneRenderer::EndScene()
	{
		ETH_PROFILE_FUNCTION();

		Flush();
	}

	void SceneRenderer::GeometryPass()
	{
		ETH_PROFILE_FUNCTION();

        
		Renderer::BeginRenderTarget(m_GeoTarget.get());
//
//		// Render entities
		for (auto& dc : m_DrawList)
		{
			Renderer::DrawMesh(m_GeometryPipeline, dc.Mesh, dc.Material, dc.Transform);
		}
        

        Renderer::EndRenderTarget();
	}

	void SceneRenderer::CompositePass()
	{
		ETH_PROFILE_FUNCTION();

//		VulkanRendererAPI::BeginRenderPass(m_CompositePipeline->GetSpecification().RenderPass, false);
//
//		auto geoFramebuffer = m_GeometryPipeline->GetSpecification().RenderPass->GetSpecification().TargetFramebuffer;
//
//		VulkanRendererAPI::DrawFullscreenQuad(m_CompositePipeline, m_CompositeMaterial);
//		VulkanRendererAPI::EndRenderPass();
	}

	void SceneRenderer::Flush()
	{
//		VulkanRendererAPI::BeginRenderCommandBuffer(m_CommandBuffer);

//		GeometryPass();

//		CompositePass();
        
        Renderer::TransitionLayout(m_GeoColor, ImageLayout::Undefined, ImageLayout::General, AccessMask::None, PipelineStage::PipeTop, AccessMask::MemoryWrite, PipelineStage::ComputeShader);
        Renderer::BeginCompute(m_MorphingPipeline, m_ComputeMat, m_ViewportWidth/8, m_ViewportHeight/8, 1);
        Renderer::TransitionLayout(m_GeoColor, ImageLayout::General, ImageLayout::PresentSRC, AccessMask::MemoryWrite, PipelineStage::ComputeShader, AccessMask::None, PipelineStage::PipeBottom);

//		VulkanRendererAPI::EndRenderCommandBuffer();
//		m_CommandBuffer->Submit();

		m_DrawList.clear();
	}

    void SceneRenderer::Shutdown()
    {
        m_MorphingPipeline->Destroy();
        m_ComputeMat->Destroy();
        m_GeometryPipeline->Destroy();
        m_GeoTarget->Destroy();
        m_GeoDepth->Destroy();
    }
}
