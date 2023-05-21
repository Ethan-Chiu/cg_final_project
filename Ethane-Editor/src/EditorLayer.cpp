#include "EditorLayer.h"
// #include "imgui/imgui.h"

#include "Ethane/Utils/PlatformUtils.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
        m_ActiveScene = CreateRef<Scene>();
        m_ViewportRenderer = CreateRef<SceneRenderer>(Renderer::GetGraphicsContext(0), m_ActiveScene);
        
        m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
	}

	void EditorLayer::OnDetach()
	{
        m_ViewportRenderer->Shutdown();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
        m_ActiveScene->OnUpdateEditor(m_ViewportRenderer, ts, m_EditorCamera);
	}

	void EditorLayer::OnEvent(Event& e)
	{
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowResizeEvent>(ETH_BIND_EVENT_FN(EditorLayer::OnResize));
	}

	void EditorLayer::OnImGuiRender()
	{
		
	}

    bool EditorLayer::OnResize(WindowResizeEvent& e)
    {
        m_ViewportRenderer->SetViewportSize(e.GetWidth(), e.GetHeight());
        return false;
    }
}
