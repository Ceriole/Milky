#pragma once

#include "Scene.h"

#include <entt.hpp>

namespace Milky {

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename Comp, typename... Args>
		Comp& AddComponent(Args&&... args)
		{
			ML_CORE_ASSERT(!HasComponent<Comp>(), "Entity already contains component!");
			Comp& component = m_Scene->m_Registry.emplace<Comp>(m_Handle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<Comp>(*this, component);
			return component;
		}
		template<typename... Comps>
		decltype(auto) GetComponent() const
		{
			ML_CORE_ASSERT(HasComponent<Comps...>(), "Entity does not contain component!");
			return m_Scene->m_Registry.get<Comps...>(m_Handle);
		}
		template<typename... Comps>
		bool HasComponent() const
		{
			return m_Scene->m_Registry.all_of<Comps...>(m_Handle);
		}
		template<typename... Comps>
		void RemoveComponent()
		{
			ML_CORE_ASSERT(HasComponent<Comps...>(), "Entity does not contain component!");
			m_Scene->m_Registry.remove<Comps...>(m_Handle);
		}

		size_t ComponentCount() const { return 0; }

		const std::string GetTag() const;

		operator bool() const { return m_Handle != entt::null; }
		operator entt::entity() const { return m_Handle; }
		operator uint32_t() const { return (uint32_t)m_Handle; }

		bool operator==(const Entity& other) const
		{
			return m_Handle == other.m_Handle && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}
	private:
		entt::entity m_Handle{ entt::null };
		Scene* m_Scene = nullptr;
	};

}
