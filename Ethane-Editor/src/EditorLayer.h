#pragma once

#include "Ethane.h"

namespace Ethane {

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;
    
        bool OnResize(WindowResizeEvent& e);
        
        bool OnMousePressed(MouseButtonPressedEvent& e);
        bool OnMouseReleased(MouseButtonReleasedEvent& e);
        bool OnKeyPressed(KeyPressedEvent& e);
        
    private:
        Ref<Scene> m_ActiveScene;
        Ref<SceneRenderer> m_ViewportRenderer;
        
        Ref<Mesh> m_Mesh;
        Ref<Material> m_Mat;
        
        EditorCamera m_EditorCamera;
        
        uint8_t m_CurrentState = 1;
        std::vector<Line> m_LineDataOne;
        std::vector<Line> m_LineDataTwo;
	};

}
