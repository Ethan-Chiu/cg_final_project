#pragma once

#include "entt.hpp"

#include "Ethane/Core/Timestep.h"
#include "Ethane/Renderer/EditorCamera.h"
#include "Ethane/Renderer/Material.h"

namespace Ethane {

	class Renderer2D;
	class SceneRenderer;
	class Entity;

	class Scene {
	public:
		Scene();
		~Scene();

		void Init();

		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdateEditor(Ref<SceneRenderer> renderer, Timestep ts, EditorCamera& camera);
		void OnUpdateRuntime(Ref<SceneRenderer> renderer, Timestep ts);
		
		void SetViewportSize(uint32_t width, uint32_t height);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();

		template<typename T>
		auto GetEntitiesWithComponent()
		{
			return m_Registry.view<T>();
		}
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_Registry;
        Ref<Material> m_TestMat;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

}
