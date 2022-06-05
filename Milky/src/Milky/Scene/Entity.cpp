#include "mlpch.h"
#include "Entity.h"

#include "Components/ComponentsBasic.h"

namespace Milky {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_Handle(handle), m_Scene(scene)
	{}

	const std::string Entity::Tag() const
	{
		return HasComponent<TagComponent>() ? GetComponent<TagComponent>().Tag : "Unnamed Entity";
	}

	TransformComponent& Entity::Transform()
	{
		return GetComponent<TransformComponent>();
	}

}
