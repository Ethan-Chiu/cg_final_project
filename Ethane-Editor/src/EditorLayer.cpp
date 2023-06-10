#include "EditorLayer.h"
#include "imgui.h"

#include "Ethane/Utils/PlatformUtils.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Ethane/Scene/Components.h"


namespace Ethane {

	// TODO: temp
	extern const std::filesystem::path g_AssetsPath;

	EditorLayer::EditorLayer()
		:Layer("EditorLayer")
	{
	}

	void EditorLayer::OnAttach()
{
        ShaderSystem::Load(AssetManager::GetBaseDirPath() + "cg_final_project/Ethane-Editor/assets/shaders/test.glsl");
        ShaderSystem::Load(AssetManager::GetBaseDirPath() + "cg_final_project/Ethane-Editor/assets/shaders/test_compute.glsl");
        
        m_ActiveScene = CreateRef<Scene>();
        m_ViewportRenderer = CreateRef<SceneRenderer>(m_ActiveScene);
        
        auto& rendererConfig = Renderer::GetRendererConfig();
        m_ViewportRenderer->SetViewportSize(rendererConfig.DefaultWindowWidth, rendererConfig.DefaultWindowHeight);
        m_ViewportWidth = rendererConfig.DefaultWindowWidth;
        m_ViewportHeight = rendererConfig.DefaultWindowHeight;
        
        m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
        m_EditorCamera.SetViewportSize(Renderer::GetRendererConfig().DefaultWindowWidth, Renderer::GetRendererConfig().DefaultWindowHeight);
        
        auto newEntity = m_ActiveScene->CreateEntity("Cube");
        m_Mesh = AssetManager::GetAssetMesh(AssetManager::GetBaseDirPath() + "cg_final_project/Ethane-Editor/resources/meshes/default/Cube.fbx");
        m_Cone = AssetManager::GetAssetMesh(AssetManager::GetBaseDirPath() + "cg_final_project/Ethane-Editor/resources/meshes/default/Cone.fbx");
        m_Torus = AssetManager::GetAssetMesh(AssetManager::GetBaseDirPath() + "cg_final_project/Ethane-Editor/resources/meshes/default/Torus.fbx");
        m_Mesh->Upload();
        m_Mat = Material::Create(ShaderSystem::Get("test").get());
        newEntity.AddComponent<MeshComponent>(m_Mesh, m_Mat);
        
        std::vector<MeshRenderData> meshes = {};
        {
            MeshRenderData& mrd = meshes.emplace_back();
            mrd.MeshRef = m_Mesh;
            mrd.Transform = glm::scale(glm::translate(glm::mat4(1.0f), {0, -10, 0}), {20, 1, 20});
            mrd.MatIndex = 1;
        }
        {
            MeshRenderData& mrd = meshes.emplace_back();
            mrd.MeshRef = m_Mesh;
            mrd.Transform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0)) * glm::scale(glm::translate(glm::mat4(1.0f), {10, -15, 0}), {10, 1, 10});
            mrd.MatIndex = 0;
        }
        {
            MeshRenderData& mrd = meshes.emplace_back();
            mrd.MeshRef = m_Mesh;
            mrd.Transform = glm::scale(glm::translate(glm::mat4(1.0f), {5, -5, 0}), {1, 1, 1});
            mrd.MatIndex = 2;
        }
        {
            MeshRenderData& mrd = meshes.emplace_back();
            mrd.MeshRef = m_Mesh;
            mrd.Transform = glm::scale(glm::translate(glm::mat4(1.0f), {0, 0, 0}), {1, 1, 1});
            mrd.MatIndex = 3;
        }
        {
            MeshRenderData& mrd = meshes.emplace_back();
            mrd.MeshRef = m_Cone;
            mrd.Transform = glm::scale(glm::translate(glm::mat4(1.0f), {6, -6, 3}), {1, 1, 1});
            mrd.MatIndex = 5;
        }
        {
            MeshRenderData& mrd = meshes.emplace_back();
            mrd.MeshRef = m_Mesh;
            mrd.Transform = glm::rotate(glm::mat4(1.0f), glm::radians(-50.0f), glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1.0f), glm::radians(70.0f), glm::vec3(1, 0, 0)) * glm::scale(glm::translate(glm::mat4(1.0f), {10, -10, 0}), {5, 1, 5});
            mrd.MatIndex = 4;
        }
        
        m_RTScene.Create(meshes);
        
        // Creating pipeline
        m_Color = Renderer::GetSwapchainTarget();
        {
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
            m_ComputeMat->SetImage("colorBuffer", m_Color);
            if(m_StoreOne != nullptr && m_StoreTwo != nullptr)
            {
                m_ComputeMat->SetData("ImageOneRefLines", m_StoreOne.get());
                m_ComputeMat->SetData("ImageTwoRefLines", m_StoreTwo.get());
                m_ComputeMat->SetData("WarpLines", m_StoreThree.get());
            }
            pipelineSpec.Shader = shader;
            m_MorphingPipeline = ComputePipeline::Create(pipelineSpec);
        }
        
        ShaderSystem::Load(AssetManager::GetBaseDirPath() + "cg_final_project/Ethane-Editor/assets/shaders/ray-trace-compute.glsl");
        
        m_RTImages.resize(3);
        ImageSpecification imageSpec;
        imageSpec.Usage = ImageUsage::Storage;
        imageSpec.Format = ImageFormat::RGBA;
        imageSpec.Width = m_ViewportWidth;
        imageSpec.Height = m_ViewportHeight;
        for (uint32_t i = 0; i < 3; i++)
        {
            m_RTImages[i] = Image2D::Create(imageSpec);
        }
        
        {
            m_TriangleStore = StorageBuffer::Create(3000 * sizeof(GpuModel::Triangle));
            m_MaterialStore = StorageBuffer::Create(30 * sizeof(GpuModel::Material));
            m_BvhStore = StorageBuffer::Create(5000 * sizeof(GpuModel::BvhNode));
            m_LightStore = StorageBuffer::Create(30 * sizeof(GpuModel::Light));
            
            ComputePipelineSpecification pipelineSpec;
            auto shader = ShaderSystem::Get("ray-trace-compute");
            m_RTMat = Material::Create(shader.get());
            m_RTMat->SetImage("targetTexture", m_RTImages[m_RTImageIndex].get(), ImageLayout::General);
            m_RTMat->SetImage("accumulationTex", m_RTImages[(m_RTImageIndex-1)%3].get(), ImageLayout::General);
            pipelineSpec.Shader = shader;
            m_RTPipeline = ComputePipeline::Create(pipelineSpec);
        }
        
        m_TriangleStore->SetData((void*)(m_RTScene.Triangles.data()), (uint32_t)(m_RTScene.Triangles.size() * sizeof(GpuModel::Triangle)));
        m_MaterialStore->SetData((void*)(m_RTScene.Materials.data()), (uint32_t)(m_RTScene.Materials.size() * sizeof(GpuModel::Material)));
        m_BvhStore->SetData((void*)(m_RTScene.BvhNodes.data()), (uint32_t)(m_RTScene.BvhNodes.size() * sizeof(GpuModel::BvhNode)));
        if (m_RTScene.Lights.size() > 0)
            m_LightStore->SetData((void*)(m_RTScene.Lights.data()), (uint32_t)(m_RTScene.Lights.size() * sizeof(GpuModel::Light)));
        
        m_RTMat->SetData("TriangleBufferObject", m_TriangleStore.get());
        m_RTMat->SetData("MaterialBufferObject", m_MaterialStore.get());
        m_RTMat->SetData("AabbBufferObject", m_BvhStore.get());
        m_RTMat->SetData("LightsBufferObject", m_LightStore.get());
        
        m_CameraPositions.resize(3);
//        ShaderSystem::Load(AssetManager::GetBaseDirPath() + "cg_final_project/Ethane-Editor/assets/shaders/denoiser.glsl");
//        {
//            ComputePipelineSpecification pipelineSpec;
//            auto shader = ShaderSystem::Get("denoiser");
//            m_DenoiseMat = Material::Create(shader.get());
//            m_DenoiseMat->SetImage("targetTexture", m_RTImages[m_RTImageIndex].get(), ImageLayout::General);
//            pipelineSpec.Shader = shader;
//            m_DenoisePipeline = ComputePipeline::Create(pipelineSpec);
//        }
//
        m_RTCommandBuffer = RenderCommandBuffer::Create(false, "raytracing");
	}

	void EditorLayer::OnDetach()
	{
        m_Mat->Destroy();
        m_Mesh->Unload();
        
        m_RTMat->Destroy();
        m_TriangleStore->Destroy();
        m_MaterialStore->Destroy();
        m_BvhStore->Destroy();
        m_LightStore->Destroy();
        m_RTPipeline->Destroy();
        
        m_StoreOne->Destroy();
        m_StoreTwo->Destroy();
        m_StoreThree->Destroy();
        m_MorphingPipeline->Destroy();
        m_ComputeMat->Destroy();
        
        m_RTCommandBuffer->Destroy();
        
        m_ViewportRenderer->Shutdown();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
        if (m_NeedResize)
        {
            m_ComputeMat->SetImage("colorBuffer", m_Color);
        }
        
        m_EditorCamera.OnUpdate(ts);
        
        auto& ubo = m_ViewportRenderer->GetGlobalUBO();
        
        if (m_CurrentMode == 0)
        {
            m_Ratio = (m_Frame % m_InterpolateFrameCount) / (float)m_InterpolateFrameCount;
            ubo.ratio = m_Ratio;
        }
        else
        {
            if (m_CurrentState == 0)
            {
                m_Ratio += m_Inc;
                if (m_Ratio >= 1) {
                    m_Ratio = 0.999;
                    m_Inc = -m_Inc;
                }
                else if (m_Ratio <= 0) {
                    m_Ratio = 0.001;
                    m_Inc = -m_Inc;
                }
            }
            ubo.ratio = m_Ratio;
        }
        
        std::vector<Line> lines;
        if (m_LineOne.size() == m_LineTwo.size())
        {
            for (int i=0; i<m_LineOne.size(); i++) {
                Line& newLine = lines.emplace_back();
                glm::vec2 M = (1 - m_Ratio)/2 * (m_LineOne[i].Start + m_LineOne[i].End) + m_Ratio/2 * (m_LineTwo[i].Start + m_LineTwo[i].End);
                newLine.Len = (1 - m_Ratio) * m_LineOne[i].Len + m_Ratio * m_LineTwo[i].Len;
                auto tmp = m_LineOne[i].End - m_LineOne[i].Start;
                float lineOne_degree = glm::atan(tmp.y, tmp.x);
                tmp = m_LineTwo[i].End - m_LineTwo[i].Start;
                float lineTwo_degree = glm::atan(tmp.y, tmp.x);
                float degree = (1 - m_Ratio) * lineOne_degree + m_Ratio * lineTwo_degree;
                newLine.Start = M - glm::vec2(0.5 * newLine.Len * cos(degree), 0.5 * newLine.Len * sin(degree));
                newLine.End = M + glm::vec2(0.5 * newLine.Len * cos(degree), 0.5 * newLine.Len * sin(degree));
            }
        }
        ubo.lineNum = (uint32_t)m_LineOne.size();
        if(lines.size() > 0) {
            m_StoreThree->SetData((void*)(lines.data()), (uint32_t)(lines.size() * sizeof(Line)));
            m_ComputeMat->SetData("WarpLines", m_StoreThree.get());
        }
        
        m_ActiveScene->OnUpdateEditor(m_ViewportRenderer, ts, m_EditorCamera);
        
        if (m_CurrentMode == 0)
        {
            if(m_Frame % m_InterpolateFrameCount == 0)
            {
                //            Timer timer;
                m_RTCommandBuffer->Wait();
                   
                m_RTImageIndex = (m_RTImageIndex+1)%3;
                m_RTMat->SetImage("targetTexture", m_RTImages[m_RTImageIndex].get(), ImageLayout::General);
                m_RTMat->SetImage("accumulationTex", m_RTImages[(m_RTImageIndex-1)%3].get(), ImageLayout::General);
                //            m_DenoiseMat->SetImage("targetTexture", m_RTImages[m_RTImageIndex].get(), ImageLayout::General);
                auto& ubo = m_ViewportRenderer->GetGlobalUBO();
                ubo.currentSample++;
                if(m_CameraPositions[(m_RTImageIndex-1)%3] != m_EditorCamera.GetPosition())
                {
                    ubo.currentSample = 0;
                }
                m_CameraPositions[m_RTImageIndex] = ubo.camPos;
                m_RTCommandBuffer->Begin();
                Renderer::TransitionLayout(m_RTImages[m_RTImageIndex].get(), ImageLayout::Undefined, ImageLayout::General, AccessMask::None, PipelineStage::PipeTop, AccessMask::MemoryWrite, PipelineStage::ComputeShader);
                Renderer::BeginCompute(m_RTPipeline, m_RTMat, m_ViewportWidth/8, m_ViewportHeight/8, 1);
                //            Renderer::BeginCompute(m_DenoisePipeline, m_DenoiseMat, m_ViewportWidth/8, m_ViewportHeight/8, 1);
                m_RTCommandBuffer->End();
                m_RTCommandBuffer->Submit();
                
                m_ComputeMat->SetImage("inputTextureOne", m_RTImages[(m_RTImageIndex-2)%3].get(), ImageLayout::General);
                m_ComputeMat->SetImage("inputTextureTwo", m_RTImages[(m_RTImageIndex-1)%3].get(), ImageLayout::General);
                // update m_LineOne, m_LineTwo
                //cam.x + t(-w/2 + x*w/sx) = a
                //cam.y + t(h/2 - y*h/sy) = b
                //cam.z - t               = c
                glm::vec3 lineStart = {0.1, 0.1, -100};
                glm::vec3 lineEnd = {0.1, 0.15, -100};
                
                Line a;
                Line b;
                a = CalcScreenLine((m_RTImageIndex-2)%3, lineStart, lineEnd);
                b = CalcScreenLine((m_RTImageIndex-1)%3, lineStart, lineEnd);
                m_LineOne.clear();
                m_LineTwo.clear();
                m_LineOne.push_back(a);
                m_LineTwo.push_back(b);
                
                m_StoreOne->SetData((void*)(m_LineOne.data()), (uint32_t)(m_LineOne.size() * sizeof(Line)));
                m_StoreTwo->SetData((void*)(m_LineTwo.data()), (uint32_t)(m_LineTwo.size() * sizeof(Line)));
            }
        }
        
        Renderer::TransitionLayout(m_Color, ImageLayout::Undefined, ImageLayout::General, AccessMask::None, PipelineStage::PipeTop, AccessMask::MemoryWrite, PipelineStage::ComputeShader);
        Renderer::BeginCompute(m_MorphingPipeline, m_ComputeMat, m_ViewportWidth/8, m_ViewportHeight/8, 1);
        Renderer::TransitionLayout(m_Color, ImageLayout::General, ImageLayout::ShaderRead, AccessMask::MemoryWrite, PipelineStage::ComputeShader, AccessMask::None, PipelineStage::PipeBottom);

        m_Frame++;
	}

    Line EditorLayer::CalcScreenLine(uint32_t index, glm::vec3 start, glm::vec3 end)
    {
        const float pi = 3.1415926535897932385;
        float h = -2 * tan(30 * pi / 180.0);
        float w = h * m_ViewportWidth / m_ViewportHeight;
        Line a;
        float t = (float)m_CameraPositions[index].z - (float)start.z;
        a.Start.x = ((float)(start.x - m_CameraPositions[index].x)/t) * ((float)m_ViewportWidth/w) + m_ViewportWidth/2.0f;
        a.Start.y = ((float)(start.y - m_CameraPositions[index].y)/t) * (-(float)m_ViewportHeight/h) + m_ViewportHeight/2.0f;
        t = (float)m_CameraPositions[index].z - (float)end.z;
        a.End.x = ((float)(end.x - m_CameraPositions[index].x)/t) * ((float)m_ViewportWidth/w)+ m_ViewportWidth/2.0f;
        a.End.y = ((float)(end.y - m_CameraPositions[index].y)/t) * (-(float)m_ViewportHeight/h) + m_ViewportHeight/2.0f;
        a.Len = glm::length(a.End - a.Start);
        return a;
    }

	void EditorLayer::OnEvent(Event& e)
	{
        m_EditorCamera.OnEvent(e);
        
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowResizeEvent>(ETH_BIND_EVENT_FN(EditorLayer::OnResize));
        if(m_CurrentMode == 1)
        {
            dispatcher.Dispatch<MouseButtonPressedEvent>(ETH_BIND_EVENT_FN(EditorLayer::OnMousePressed));
            dispatcher.Dispatch<MouseButtonReleasedEvent>(ETH_BIND_EVENT_FN(EditorLayer::OnMouseReleased));
        }
        dispatcher.Dispatch<KeyPressedEvent>(ETH_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
	}

    void EditorLayer::CleanLines()
    {
        for (auto&& l: m_LineOne)
        {
            l.Start = glm::vec2(0);
            l.End = glm::vec2(0);
            l.Len = 0;
        }
        for (auto&& l: m_LineTwo)
        {
            l.Start = glm::vec2(0);
            l.End = glm::vec2(0);
            l.Len = 0;
        }
        m_StoreOne->SetData((void*)(m_LineOne.data()), (uint32_t)(m_LineOne.size() * sizeof(Line)));
        m_StoreTwo->SetData((void*)(m_LineTwo.data()), (uint32_t)(m_LineTwo.size() * sizeof(Line)));
        m_LineOne.clear();
        m_LineTwo.clear();
    }

	void EditorLayer::OnImGuiRender()
	{
        // Create ImGui window
        ImGui::Begin("Control Panel");

        if(ImGui::Button("RayTracing")) {
            m_CurrentMode = 0;
        }
        if(ImGui::Button("ImageMorphing")) {
            m_CurrentMode = 1;
            m_ComputeMat->SetImage("inputTextureOne", m_SampleTexOne);
            m_ComputeMat->SetImage("inputTextureTwo", m_SampleTexTwo);
            m_ComputeMat->SetData("ImageOneRefLines", m_StoreOne.get());
            m_ComputeMat->SetData("ImageTwoRefLines", m_StoreTwo.get());
            CleanLines();
            m_CurrentState = 1;
            m_Ratio = 0;
        }
        if (m_CurrentMode == 1)
        {
            if(ImGui::Button("Image 1")) {
                m_CurrentState = 1;
                m_Ratio = 0;
            }
            if(ImGui::Button("Image 2")) {
                m_CurrentState = 2;
                m_Ratio = 1;
            }
            if(ImGui::SliderFloat("ratio", &m_Ratio,0.f,1.f)) {
                
            }
            if(ImGui::Button("Clear")) {
                CleanLines();
            }
        }
        ImGui::End();
	}

    bool EditorLayer::OnResize(WindowResizeEvent& e)
    {
        m_ViewportRenderer->SetViewportSize(e.GetWidth(), e.GetHeight());
        m_EditorCamera.SetViewportSize(e.GetWidth(), e.GetHeight());
        m_ViewportWidth = e.GetWidth();
        m_ViewportHeight = e.GetHeight();
        
        m_NeedResize = true;
        return false;
    }

    bool EditorLayer::OnMousePressed(MouseButtonPressedEvent& e)
    {
        if(e.GetMouseButton() == Mouse::ButtonLeft)
        {
            const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
            glm::vec2 pos = Renderer::ToPixel(mouse);
            ETH_CORE_INFO("{0} {1}", pos.x, pos.y);
            
            if (m_CurrentState == 1) {
                m_LineOne.push_back(Line(pos, pos, 0));
            } else if (m_CurrentState == 2) {
                m_LineTwo.push_back(Line(pos, pos, 0));
            }
        }
        return false;
    }

    bool EditorLayer::OnMouseReleased(MouseButtonReleasedEvent &e)
    {
        if(e.GetMouseButton() == Mouse::ButtonLeft)
        {
            const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
            glm::vec2 pos = Renderer::ToPixel(mouse);
            ETH_CORE_INFO("{0} {1}", pos.x, pos.y);
            
            if (m_CurrentState == 1) {
                Line& l = m_LineOne.back();
                l.End = pos;
                l.Len = glm::distance(l.Start, l.End);
                m_StoreOne->SetData((void*)(m_LineOne.data()), (uint32_t)(m_LineOne.size() * sizeof(Line)));
                m_ComputeMat->SetData("ImageOneRefLines", m_StoreOne.get());
            } else if (m_CurrentState == 2) {
                Line& l = m_LineTwo.back();
                l.End = pos;
                l.Len = glm::distance(l.Start, l.End);
                m_StoreTwo->SetData((void*)(m_LineTwo.data()), (uint32_t)(m_LineTwo.size() * sizeof(Line)));
                m_ComputeMat->SetData("ImageTwoRefLines", m_StoreTwo.get());
            }
        }
        return false;
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
    {
        //Shortcuts
        if(e.GetRepeatCount() > 0)
            return false;
    
        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
        bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
        switch (e.GetKeyCode())
        {
            case Key::D1:
            {
                m_CurrentState = 1;
                m_Ratio = 0;
                break;
            }
            case Key::D2:
            {
                m_CurrentState = 2;
                m_Ratio = 1;
                break;
            }
            case Key::Enter:
            {
                m_CurrentState = 0;
                m_Ratio = 0;
                break;
            }
            default:
                break;
        }
        return false;
    }

}
