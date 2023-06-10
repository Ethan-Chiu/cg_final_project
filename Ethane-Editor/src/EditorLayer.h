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
        Line CalcScreenLine(uint32_t index, glm::vec3 start, glm::vec3 end);
        void CleanLines();
        
    private:
        Ref<Scene> m_ActiveScene;
        Ref<SceneRenderer> m_ViewportRenderer;
        
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        
        Ref<Mesh> m_Mesh;
        Ref<Mesh> m_Cone;
        Ref<Mesh> m_Torus;
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
        std::vector<Ref<Image2D>> m_RTImages;
        uint32_t m_RTImageIndex = 0;
        uint32_t m_InterpolateFrameCount = 5;
        std::vector<glm::vec3> m_CameraPositions;
        
        Ref<ComputePipeline> m_DenoisePipeline = nullptr;
        Ref<Material> m_DenoiseMat = nullptr;
        
        uint8_t m_CurrentMode = 1;
        float m_Ratio = 0;
        float m_Inc = 0.005;
        
        bool m_NeedResize = false;
        
        EditorCamera m_EditorCamera;
	};

}
