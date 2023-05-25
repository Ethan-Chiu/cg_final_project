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
        
        m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
        m_EditorCamera.SetViewportSize(Renderer::GetRendererConfig().DefaultWindowWidth, Renderer::GetRendererConfig().DefaultWindowHeight);
        
        auto newEntity = m_ActiveScene->CreateEntity("Cube");
        m_Mesh = AssetManager::GetAssetMesh("/Users/ethan/ethans_folder/Program_dev/cg_final_project/Ethane-Editor/resources/meshes/default/Cube.fbx"); // TODO
        m_Mesh->Upload();
        m_Mat = Material::Create(ShaderSystem::Get("test").get());
        newEntity.AddComponent<MeshComponent>(m_Mesh, m_Mat);
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
}
