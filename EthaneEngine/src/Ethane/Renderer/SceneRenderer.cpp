#include "ethpch.h"
#include "SceneRenderer.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Ethane/Asset/AssetManager.h"
#include "Ethane/Systems/ShaderSystem.h"
#include "Ethane/Systems/TextureSystem.h"

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
//		{
//            m_GeoColor = Renderer::GetSwapchainTarget();
//            imageSpec.DebugName = "GeoDepth";
//            imageSpec.Format = ImageFormat::DEPTH32F;
//            m_GeoDepth = Image2D::Create(imageSpec);
//
//			RenderTargetSpecification geoTargetSpec;
//            geoTargetSpec.Attachments = { m_GeoColor, m_GeoDepth.get() };
//            geoTargetSpec.IsTargetImage = true;
//            geoTargetSpec.ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
//            geoTargetSpec.DebugName = "Geometry";
//            geoTargetSpec.Width = rendererConfig.DefaultWindowWidth;
//            geoTargetSpec.Height = rendererConfig.DefaultWindowHeight;
//            geoTargetSpec.SwapChainTarget = true;
//			m_GeoTarget = RenderTarget::Create(geoTargetSpec);
//
//			PipelineSpecification pipelineSpecification;
//			pipelineSpecification.Layout = {
//				{ ShaderDataType::Float3, "a_Position" },
//				{ ShaderDataType::Float3, "a_Normal" },
//				{ ShaderDataType::Float3, "a_Tangent" },
//				{ ShaderDataType::Float3, "a_Binormal" },
//				{ ShaderDataType::Float2, "a_TexCoord" },
//			};
//            auto shader = ShaderSystem::Get("test");
//            pipelineSpecification.Shader = shader;
//            pipelineSpecification.RenderPass = m_GeoTarget->GetRenderPass();
//			m_GeometryPipeline = Pipeline::Create(pipelineSpecification);
//		}

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
		
//            m_GeoTarget->Resize(m_ViewportWidth, m_ViewportHeight);
            
//            m_GeoColor = Renderer::GetSwapchainTarget();
//            m_ComputeMat->SetImage("colorBuffer", m_GeoColor);
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

        ubo.camPos = sceneCamera.GetPosition();
        
        Renderer::SetGlobalUniformBuffer(0, (void*)&(ubo), sizeof(UBO));
        Renderer::SetGlobalUniformBuffer(1, (void*)&(cameraData.ViewProjection), sizeof(glm::mat4));
	}

	void SceneRenderer::SubmitMesh(Mesh* mesh, Material* material, const glm::mat4& transform)
	{
		// TODO: Culling, sorting, etc.
//		m_DrawList.push_back({ mesh, transform, material});
	}

	void SceneRenderer::SubmitSelectedMesh(Mesh* mesh, const glm::mat4& transform) //, Ref<Material> material
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
			Renderer::DrawMesh(m_GeometryPipeline, dc.MeshPtr, dc.MaterialPtr, dc.Transform);
		}
        

        Renderer::EndRenderTarget();
	}

	void SceneRenderer::Flush()
	{
//		VulkanRendererAPI::BeginRenderCommandBuffer(m_CommandBuffer);

//		GeometryPass();

//		VulkanRendererAPI::EndRenderCommandBuffer();
//		m_CommandBuffer->Submit();

		m_DrawList.clear();
	}

    void SceneRenderer::Shutdown()
    {
    }
}
