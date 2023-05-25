#include "ethpch.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"

// Renderer
#include "Ethane/Renderer/Renderer.h"
#include "Ethane/Renderer/SceneRenderer.h"

#include <glm/glm.hpp>


namespace Ethane {

	Scene::Scene()
	{
		Init();
	}

	Scene::~Scene()
	{
	}

	void Scene::Init()
	{

	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>(name.empty() ? "Entity" : name);
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnRuntimeStart()
	{
	}

	void Scene::OnRuntimeStop()
	{
	}

	void Scene::OnUpdateRuntime(Ref<SceneRenderer> renderer, Timestep ts)
	{
		// Update Scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
			{
				if (!nsc.Instance)
				{
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->m_Entity = Entity{ entity, this };
					nsc.Instance->OnCreate();
				}

				nsc.Instance->OnUpdate(ts);
			});
		}

		// Find Primary Camera
		SceneCamera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}

		// Render
		if (mainCamera)
		{
			mainCamera->SetViewportSize(m_ViewportWidth, m_ViewportHeight);

			renderer->BeginScene(*mainCamera, cameraTransform);

			auto group = m_Registry.group<MeshComponent>(entt::get<TransformComponent>);
			for (auto entity : group)
			{
				auto [meshComponent, transformComponent] = group.get<MeshComponent, TransformComponent>(entity);
				if (meshComponent.MeshRef)
				{
					// glm::mat4 transform = GetTransformRelativeToParent(Entity{ entity, this });

					renderer->SubmitMesh(meshComponent.MeshRef, meshComponent.MatRef, transformComponent.GetTransform());
				}
			}

			renderer->EndScene();
		}
	}

	void Scene::OnUpdateEditor(Ref<SceneRenderer> renderer, Timestep ts, EditorCamera& camera)
	{
		renderer->BeginScene(camera, camera.GetViewMatrix());

		auto group = m_Registry.group<MeshComponent>(entt::get<TransformComponent>);
		for (auto entity : group)
		{
			auto [meshComponent, transformComponent] = group.get<MeshComponent, TransformComponent>(entity);
			if (meshComponent.MeshRef)
			{
				renderer->SubmitMesh(meshComponent.MeshRef, meshComponent.MatRef, transformComponent.GetTransform());
			}
		}

		renderer->EndScene();
	}

	void Scene::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;
	}

	// TODO: this function is not used right now
	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
			{
				cameraComponent.Camera.SetViewportSize(width, height);
			}
		}
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{ entity, this };
		}
		return {};
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent& component)
	{
	}

    template<typename T>
    void Scene::OnComponentAdded(Entity entity, T& component)
    {
        ETH_CORE_ASSERT(false);
    }
}
