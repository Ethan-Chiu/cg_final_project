#include "ethpch.h"
#include "Entity.h"

namespace Ethane {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{}

}