#include "EditorLayer.h"
// #include "imgui/imgui.h"

#include "Ethane/Utils/PlatformUtils.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Ethane/Scene/Components.h"
#include "Ethane/Math/Math.h"

namespace Ethane {

	// TODO: temp
	extern const std::filesystem::path g_AssetsPath;

	EditorLayer::EditorLayer()
		:Layer("EditorLayer")
	{
	}

	void EditorLayer::OnAttach()
	{
        ShaderSystem::Load("/Users/ethan/ethans_folder/Program_dev/cg_final_project/Ethane-Editor/assets/shaders/test.glsl");
        ShaderSystem::Load("/Users/ethan/ethans_folder/Program_dev/cg_final_project/Ethane-Editor/assets/shaders/test_compute.glsl");
        
        m_ActiveScene = CreateRef<Scene>();
        m_ViewportRenderer = CreateRef<SceneRenderer>(m_ActiveScene);
        
        auto& rendererConfig = Renderer::GetRendererConfig();
        m_ViewportRenderer->SetViewportSize(rendererConfig.DefaultWindowWidth, rendererConfig.DefaultWindowHeight);
        
        m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
        m_EditorCamera.SetViewportSize(Renderer::GetRendererConfig().DefaultWindowWidth, Renderer::GetRendererConfig().DefaultWindowHeight);
        
        auto newEntity = m_ActiveScene->CreateEntity("Cube");
        m_Mesh = AssetManager::GetAssetMesh("/Users/ethan/ethans_folder/Program_dev/cg_final_project/Ethane-Editor/resources/meshes/default/Cube.fbx"); // TODO
        m_Mesh->Upload();
        m_Mat = Material::Create(ShaderSystem::Get("test").get());
        newEntity.AddComponent<MeshComponent>(m_Mesh, m_Mat);
        
        m_LineDataOne.push_back(Line({0, 0}, {30, 30}));
        m_LineDataTwo.push_back(Line({30, 30}, {60, 60}));
        m_ViewportRenderer->SetLineOneData(m_LineDataOne);
        m_ViewportRenderer->SetLineTwoData(m_LineDataTwo);
	}

	void EditorLayer::OnDetach()
	{
        m_Mat->Destroy();
        m_Mesh->Unload();
        
        m_ViewportRenderer->Shutdown();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
        m_EditorCamera.OnUpdate(ts);
        m_ActiveScene->OnUpdateEditor(m_ViewportRenderer, ts, m_EditorCamera);
	}

	void EditorLayer::OnEvent(Event& e)
	{
        m_EditorCamera.OnEvent(e);
        
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowResizeEvent>(ETH_BIND_EVENT_FN(EditorLayer::OnResize));
        dispatcher.Dispatch<MouseButtonPressedEvent>(ETH_BIND_EVENT_FN(EditorLayer::OnMousePressed));
        dispatcher.Dispatch<MouseButtonReleasedEvent>(ETH_BIND_EVENT_FN(EditorLayer::OnMouseReleased));
        dispatcher.Dispatch<KeyPressedEvent>(ETH_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
	}

	void EditorLayer::OnImGuiRender()
	{
		
	}

    bool EditorLayer::OnResize(WindowResizeEvent& e)
    {
        m_ViewportRenderer->SetViewportSize(e.GetWidth(), e.GetHeight());
        m_EditorCamera.SetViewportSize(e.GetWidth(), e.GetHeight());
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
                m_LineDataOne.push_back(Line(pos, pos));
            } else if (m_CurrentState == 2) {
                m_LineDataTwo.push_back(Line(pos, pos));
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
                m_LineDataOne.back().End = pos;
                m_ViewportRenderer->SetLineOneData(m_LineDataOne);
            } else if (m_CurrentState == 2) {
                m_LineDataTwo.back().End = pos;
                m_ViewportRenderer->SetLineTwoData(m_LineDataTwo);
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
                break;
            }
            case Key::D2:
            {
                m_CurrentState = 2;
                ETH_CORE_INFO("set to 2");
                break;
            }
            default:
                break;
        }
        return false;
    }

}
