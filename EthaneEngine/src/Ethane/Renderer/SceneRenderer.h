#pragma once

#include "Renderer.h"
#include "Image.h"
#include "Mesh.h"
#include "Ethane/Scene/Scene.h"
#include "RenderTarget.h"


namespace Ethane {

	class Scene;

	struct SceneRendererOptions
	{
		bool ShowGrid = true;
	};

    struct Line {
        alignas(8) glm::vec2 Start = glm::vec2();
        alignas(8) glm::vec2 End = glm::vec2();
        alignas(16) float Len = 0;

        Line(glm::vec2 s, glm::vec2 e, float len) : Start(s), End(e), Len(len) {}
        Line() = default;
    };

	class SceneRenderer
	{
	public:
		SceneRenderer(Ref<Scene> scene);

		void Init();
        void Shutdown();

		void SetViewportSize(uint32_t width, uint32_t height);

		void BeginScene(const Camera& camera, const glm::mat4& viewMatrix);
		void EndScene();

		void SubmitMesh(Mesh* mesh, Material* material, const glm::mat4& transform = glm::mat4(1.0f));
		void SubmitSelectedMesh(Mesh* mesh, const glm::mat4& transform = glm::mat4(1.0f)); //, Ref<Material> Material = nullptr

        // temp
        void SetLineOneData(const std::vector<Line>& data) {
            m_LineOne = data;
            m_StoreOne->SetData((void*)(data.data()), (uint32_t)(data.size() * sizeof(Line)));
            m_ComputeMat->SetData("ImageOneRefLines", m_StoreOne.get());
        }
        
        void SetLineTwoData(const std::vector<Line>& data) {
            m_LineTwo = data;
            m_StoreTwo->SetData((void*)(data.data()), (uint32_t)(data.size() * sizeof(Line)));
            m_ComputeMat->SetData("ImageTwoRefLines", m_StoreTwo.get());
        }
        
        void SetCurrentState(uint8_t state) {
            m_CurrentState = state;
        }
        
		// Getter
		SceneRendererOptions& GetOptions() { return m_Options; }

	private:
		void Flush();

		void GeometryPass();
		
		void CompositePass();

		

	private:
		SceneRendererOptions m_Options;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		float m_InvViewportWidth = 0.f, m_InvViewportHeight = 0.f;
		bool m_NeedResize = false;

		Ref<Scene> m_Scene;

		struct UBGlobal
		{
			glm::mat4 ViewProjection;
			glm::vec4 AmbientColor;
			glm::vec3 ViewPosition;
		} m_GlobalUB;

		//TODO: temporary 
		struct UBLocal
		{
			glm::vec4 DiffuseColor;
			float Shininess;
		};

		struct UBCamera
		{
			glm::mat4 ViewProjection;
			glm::mat4 InverseViewProjection;
			glm::mat4 Projection;
			glm::mat4 View;
		} CameraDataUB;

        struct UBO
        {
            alignas(4) float ratio;
            alignas(4) u_int32_t currentSample;
            alignas(4) u_int32_t lineNum;
        } ubo;
        
        uint8_t m_CurrentState = 1;
        
        std::vector<Line> m_LineOne = {};
        std::vector<Line> m_LineTwo = {};
        
        Ref<StorageBuffer> m_StoreOne = nullptr;
        Ref<StorageBuffer> m_StoreTwo = nullptr;
        Ref<StorageBuffer> m_StoreThree = nullptr;
        
        TargetImage* m_GeoColor = nullptr;
        Ref<Image2D> m_GeoDepth = nullptr;
        Scope<RenderTarget> m_GeoTarget = nullptr;
		Ref<Pipeline> m_GeometryPipeline;
        Texture2D* m_SampleTexOne = nullptr;
        Texture2D* m_SampleTexTwo = nullptr;
        Ref<Material> m_ComputeMat = nullptr;
        Ref<ComputePipeline> m_MorphingPipeline = nullptr;
        
		struct DrawCommand
		{
			Mesh* MeshPtr = nullptr;
			glm::mat4 Transform;
			Material* MaterialPtr = nullptr;
		};
		std::vector<DrawCommand> m_DrawList;
		std::vector<DrawCommand> m_SelectedMeshDrawList;
	};

}
