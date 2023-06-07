#pragma once

#include "Ethane.h"
#include "RTScene.h"

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
        
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        
        Ref<Mesh> m_Mesh;
        Ref<Material> m_Mat;
        
        RTScene m_RTScene;
        TargetImage* m_Color;
        Ref<Material> m_RTMat = nullptr;
        Ref<ComputePipeline> m_RTPipeline = nullptr;
        Ref<StorageBuffer> m_TriangleStore = nullptr;
        Ref<StorageBuffer> m_MaterialStore = nullptr;
        Ref<StorageBuffer> m_BvhStore = nullptr;
        Ref<StorageBuffer> m_LightStore = nullptr;
        
        std::vector<Line> m_LineOne = {};
        std::vector<Line> m_LineTwo = {};
        Texture2D* m_SampleTexOne = nullptr;
        Texture2D* m_SampleTexTwo = nullptr;
        Ref<Material> m_ComputeMat = nullptr;
        Ref<ComputePipeline> m_MorphingPipeline = nullptr;
        Ref<StorageBuffer> m_StoreOne = nullptr;
        Ref<StorageBuffer> m_StoreTwo = nullptr;
        Ref<StorageBuffer> m_StoreThree = nullptr;
        uint8_t m_CurrentState = 1;
        
        uint32_t m_Frame = 0;
        Ref<RenderCommandBuffer> m_RTCommandBuffer;
//        std::vector
        
        bool m_NeedResize = false;
        
        EditorCamera m_EditorCamera;
	};

}
