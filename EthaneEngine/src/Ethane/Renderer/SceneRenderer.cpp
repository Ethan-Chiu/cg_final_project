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
//            m_LineDataOne.push_back(Line(glm::vec2{0, 0}, glm::vec2(30, 30)));
//            m_LineDataTwo.push_back(Line(glm::vec2{30, 30}, glm::vec2(60, 60)));
            ETH_CORE_INFO("size of line is {0}", sizeof(Line));
            
            m_StoreOne = StorageBuffer::Create(30 * sizeof(Line));
            m_StoreTwo = StorageBuffer::Create(30 * sizeof(Line));
            m_StoreThree = StorageBuffer::Create(30 * sizeof(Line));
            
            ComputePipelineSpecification pipelineSpec;
            auto shader = ShaderSystem::Get("test_compute");
            m_ComputeMat = Material::Create(shader.get());
            
            m_SampleTexOne = TextureSystem::GetTexture(AssetManager::GetBaseDirPath() + "cg_final_project/Ethane-Editor/assets/textures/1.jpg", ImageUsage::Storage);
            m_SampleTexTwo = TextureSystem::GetTexture(AssetManager::GetBaseDirPath() + "cg_final_project/Ethane-Editor/assets/textures/2.jpg", ImageUsage::Storage);
            m_ComputeMat->SetImage("inputTextureOne", m_SampleTexOne);
            m_ComputeMat->SetImage("inputTextureTwo", m_SampleTexTwo);
            m_ComputeMat->SetImage("colorBuffer", m_GeoColor);
            if(m_StoreOne != nullptr && m_StoreTwo != nullptr)
            {
                m_ComputeMat->SetData("ImageOneRefLines", m_StoreOne.get());
                m_ComputeMat->SetData("ImageTwoRefLines", m_StoreTwo.get());
                m_ComputeMat->SetData("WarpLines", m_StoreThree.get());
            }
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

        float ratio = ubo.ratio;
        if(m_CurrentState == 1)
            ratio = 0;
        else if (m_CurrentState == 2)
            ratio = 1;
        else {
            ratio += 0.01;
            if (ratio >= 1) {
                ratio = 0.01;
            }
        }
        
        ubo.ratio = ratio;
        ubo.currentSample++;
        
        std::vector<Line> lines;
        if (m_LineOne.size() == m_LineTwo.size())
        {
            for (int i=0; i<m_LineOne.size(); i++) {
                Line& newLine = lines.emplace_back();
                glm::vec2 M = (1 - ratio)/2 * (m_LineOne[i].Start + m_LineOne[i].End) + ratio/2 * (m_LineTwo[i].Start + m_LineTwo[i].End);
                newLine.Len = (1 - ratio) * m_LineOne[i].Len + ratio * m_LineTwo[i].Len;
                auto tmp = m_LineOne[i].End - m_LineOne[i].Start;
                float lineOne_degree = glm::atan(tmp.y, tmp.x);
                tmp = m_LineTwo[i].End - m_LineTwo[i].Start;
                float lineTwo_degree = glm::atan(tmp.y, tmp.x);
                float degree = (1 - ratio) * lineOne_degree + ratio * lineTwo_degree;
                newLine.Start = M - glm::vec2(0.5 * newLine.Len * cos(degree), 0.5 * newLine.Len * sin(degree));
                newLine.End = M + glm::vec2(0.5 * newLine.Len * cos(degree), 0.5 * newLine.Len * sin(degree));
            }
        }
        ubo.lineNum = (uint32_t)m_LineOne.size();

        if(lines.size() > 0) {
            m_StoreThree->SetData((void*)(lines.data()), (uint32_t)(lines.size() * sizeof(Line)));
            m_ComputeMat->SetData("WarpLines", m_StoreThree.get());
        }
        
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
        Renderer::TransitionLayout(m_GeoColor, ImageLayout::General, ImageLayout::ShaderRead, AccessMask::MemoryWrite, PipelineStage::ComputeShader, AccessMask::None, PipelineStage::PipeBottom);

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
