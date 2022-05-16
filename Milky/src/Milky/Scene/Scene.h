#pragma once

#include "Milky/Core/Timestep.h"

#include <entt.hpp>

namespace Milky {

	class Entity;

	class Scene {
		friend class Entity;
		friend class ScenePanels;
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnUpdate(Timestep ts);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();
	private:
		template<typename Comp>
		void OnComponentAdded(Entity entity, Comp& component);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	};

}
