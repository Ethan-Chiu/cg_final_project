#include "ethpch.h"
#include "SceneRenderer.h"

#include <glm/gtc/matrix_transform.hpp>

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
            m_LineDataOne.push_back(Line(glm::vec2{0, 0}, glm::vec2(30, 30)));
            m_LineDataTwo.push_back(Line(glm::vec2{30, 30}, glm::vec2(60, 60)));
            
            m_StoreOne = StorageBuffer::Create((void*)(m_LineDataOne.data()), (uint32_t)(m_LineDataOne.size() * sizeof(Line)));
            m_StoreTwo = StorageBuffer::Create((void*)(m_LineDataTwo.data()), (uint32_t)(m_LineDataTwo.size() * sizeof(Line)));
            
            ComputePipelineSpecification pipelineSpec;
            auto shader = ShaderSystem::Get("test_compute");
            m_ComputeMat = Material::Create(shader.get());
            m_SampleTexOne = TextureSystem::GetTexture("/Users/ethan/ethans_folder/Program_dev/cg_final_project/Ethane-Editor/assets/textures/1.jpg", ImageUsage::Storage);
            m_SampleTexTwo = TextureSystem::GetTexture("/Users/ethan/ethans_folder/Program_dev/cg_final_project/Ethane-Editor/assets/textures/2.jpg", ImageUsage::Storage);
            m_ComputeMat->SetImage("inputTextureOne", m_SampleTexOne);
            m_ComputeMat->SetImage("inputTextureTwo", m_SampleTexTwo);
            m_ComputeMat->SetImage("colorBuffer", m_GeoColor);
            m_ComputeMat->SetData("ImageOneRefLines", m_StoreOne.get());
            m_ComputeMat->SetData("ImageTwoRefLines", m_StoreTwo.get());
            pipelineSpec.Shader = shader;
            m_MorphingPipeline = ComputePipeline::Create(pipelineSpec);
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

        float time = 0.5;
        
        Renderer::SetGlobalUniformBuffer(0, (void*)&(time), sizeof(float));
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
