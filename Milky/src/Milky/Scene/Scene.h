#pragma once

#include "Milky/Core/Timestep.h"
#include "Milky/Renderer/EditorCamera.h"

#include <entt.hpp>

namespace Milky {

	class Entity;

	class Scene {
		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	public:
		Scene(const Scene& scene) = default;
		Scene();
		~Scene();

		Entity CreateEmptyEntity();
		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();

		template<typename Comp>
		void OnComponentAdded(Entity entity, Comp& component);

		size_t GetNumEntites();
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	};

}
