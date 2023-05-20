#include "ethpch.h"
#include "SceneRenderer.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Ethane/Asset/ShaderLibrary.h"

// TODO: temp
#include "Vulkan/VulkanRendererAPI.h"
#include "Vulkan/VulkanFramebuffer.h"


namespace Ethane {

	SceneRenderer::SceneRenderer(const GraphicsContext* ctx, Ref<Scene> scene)
		: m_Context(ctx), m_Scene(scene)
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
//            imageSpec.DebugName = "GeoColor";
//            imageSpec.Format = ImageFormat::BGRA;
//            m_GeoColor = Image2D::Create(m_Context, imageSpec);
//            imageSpec.DebugName = "GeoDepth";
//            imageSpec.Format = ImageFormat::Depth;
//            m_GeoDepth = Image2D::Create(m_Context, imageSpec);
//
//			FramebufferSpecification geoFramebufferSpec;
//			geoFramebufferSpec.Attachments = { m_GeoColor.get(), m_GeoDepth.get() };
//			geoFramebufferSpec.ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
//			geoFramebufferSpec.DebugName = "Geometry";
//            geoFramebufferSpec.Width = rendererConfig.DefaultWindowWidth;
//            geoFramebufferSpec.Height = rendererConfig.DefaultWindowHeight;
//			Ref<Framebuffer> framebuffer = Framebuffer::Create(m_Context, geoFramebufferSpec);
//
//			PipelineSpecification pipelineSpecification;
//			pipelineSpecification.Layout = {
//				{ ShaderDataType::Float3, "a_Position" },
//				{ ShaderDataType::Float3, "a_Normal" },
//				{ ShaderDataType::Float3, "a_Tangent" },
//				{ ShaderDataType::Float3, "a_Binormal" },
//				{ ShaderDataType::Float2, "a_TexCoord" },
//			};
//			pipelineSpecification.Shader = ShaderLibrary::Get("PBR_static");
//            pipelineSpecification.RenderPass = framebuffer->GetRenderPass();
//			m_GeometryPipeline = Pipeline::Create(m_Context, pipelineSpecification);

//			m_TestDiffuse = Texture2D::Create("assets/textures/FloorSandStone/cobblestone.png");
//			m_TestSpecular = Texture2D::Create("assets/textures/FloorSandStone/cobblestone_SPEC.png");
//			m_TestNormal = Texture2D::Create("assets/textures/FloorSandStone/cobblestone_NRM.png");
			// TODO: test remove
//			m_testMaterial = Material::Create(ShaderLibrary::Get("PBR_static"), "tset Geo material");
//			m_testMaterial->Set("u_DiffuseSampler", m_TestDiffuse);
//			m_testMaterial->Set("u_SpecularSampler", m_TestSpecular);
//			m_testMaterial->Set("u_NormalSampler", m_TestNormal);
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
		
			m_GeoFramebuffer->Resize(m_ViewportWidth, m_ViewportHeight);
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

//		VulkanRendererAPI::SetUniformBuffer(0, 0, &globalData, sizeof(globalData), 0);
//		VulkanRendererAPI::SetUniformBuffer(0, 1, &localUB, sizeof(localUB), 0);
	}

	void SceneRenderer::SubmitMesh(Ref<Mesh> mesh, const glm::mat4& transform) // , Ref<Material> material
	{
		// TODO: Culling, sorting, etc.
		m_DrawList.push_back({ mesh, transform }); // , material
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

//		VulkanRendererAPI::BeginRenderPass(m_GeometryPipeline->GetSpecification().RenderPass);
//
//		// Render entities
//		for (auto& dc : m_DrawList)
//		{
//			// VulkanRendererAPI::DrawMesh(m_GeometryPipeline, dc.Mesh, dc.Material, dc.Transform);
//			VulkanRendererAPI::DrawMesh(m_GeometryPipeline, dc.Mesh, m_testMaterial, dc.Transform);
//		}
//
//		// Grid
//		if (GetOptions().ShowGrid)
//		{
//			const glm::mat4 transform = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(8.0f));
//			VulkanRendererAPI::DrawQuad(m_GridPipeline, m_GridMaterial, transform);
//		}
//
//		VulkanRendererAPI::EndRenderPass();
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

		GeometryPass();

		CompositePass();

//		VulkanRendererAPI::EndRenderCommandBuffer();
//		m_CommandBuffer->Submit();

		m_DrawList.clear();
	}
}
