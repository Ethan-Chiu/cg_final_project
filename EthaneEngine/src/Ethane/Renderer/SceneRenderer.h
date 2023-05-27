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

	class SceneRenderer
	{
	public:
		SceneRenderer(Ref<Scene> scene);

		void Init();
        void Shutdown();

		void SetViewportSize(uint32_t width, uint32_t height);

		void BeginScene(const Camera& camera, const glm::mat4& viewMatrix);
		void EndScene();

		void SubmitMesh(Ref<Mesh> mesh, Ref<Material> material, const glm::mat4& transform = glm::mat4(1.0f));
		void SubmitSelectedMesh(Ref<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f)); //, Ref<Material> Material = nullptr

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

        TargetImage* m_GeoColor = nullptr;
        Ref<Image2D> m_GeoDepth = nullptr;
        Scope<RenderTarget> m_GeoTarget = nullptr;
        
        

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

		struct UBScreenData
		{
			glm::vec2 InvFullResolution;
			glm::vec2 FullResolution;
		} ScreenDataUB;

		struct UBRendererData
		{
			glm::vec4 CascadeSplits;
			uint32_t TilesCountX{ 0 };
			bool ShowCascades = false;
			char Padding0[3] = { 0,0,0 }; // Bools are 4-bytes in GLSL
			bool SoftShadows = true;
			char Padding1[3] = { 0,0,0 };
			float LightSize = 0.5f;
			float MaxShadowDistance = 200.0f;
			float ShadowFade = 1.0f;
			bool CascadeFading = true;
			char Padding2[3] = { 0,0,0 };
			float CascadeTransitionFade = 1.0f;
			bool ShowLightComplexity = false;
			char Padding3[3] = { 0,0,0 };
		} RendererDataUB;

        
		Ref<Pipeline> m_GeometryPipeline;
        Ref<Material> m_ComputeMat;
        Ref<ComputePipeline> m_MorphingPipeline;
        
		struct DrawCommand
		{
			Ref<Mesh> Mesh;
			glm::mat4 Transform;
			Ref<Material> Material;
		};
		std::vector<DrawCommand> m_DrawList;
		std::vector<DrawCommand> m_SelectedMeshDrawList;
	};

}
