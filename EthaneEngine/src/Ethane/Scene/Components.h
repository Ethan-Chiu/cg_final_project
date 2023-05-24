#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABEL_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "SceneCamera.h"
#include "ScriptableEntity.h"
#include "Ethane/Renderer/Mesh.h"
#include "Ethane/Renderer/Texture.h"
#include "Ethane/Renderer/Material.h"

namespace Ethane {

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			:Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			:Translation(translation) {}
		
		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
			
			return glm::scale(glm::translate(glm::mat4(1.0f), Translation) * rotation, Scale);
		}
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity*(*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};

	struct MeshComponent
	{
		Ref<Mesh> MeshRef = nullptr;
        Ref<Material> MatRef = nullptr;
        // Ref<Ethane::MaterialTable> MaterialTable = Ref<Hazel::MaterialTable>::Create();
	
        MeshComponent() = default;
        MeshComponent(const MeshComponent&) = default;
		MeshComponent(const Ref<Mesh>& mesh, const Ref<Material> material)
			: MeshRef(mesh), MatRef(material) {}
	
		operator Ref<Mesh>() { return MeshRef; }
	};
}
