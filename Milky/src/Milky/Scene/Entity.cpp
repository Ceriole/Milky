#include "mlpch.h"
#include "Entity.h"

#include "Components.h"

namespace Milky {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_Handle(handle), m_Scene(scene)
	{}

	std::string Entity::GetTag()
	{
		return HasComponent<TagComponent>() ? GetComponent<TagComponent>().Tag : "Unnamed Entity";
	}

}
