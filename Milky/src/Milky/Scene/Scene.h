#pragma once

#include "Milky/Core/UUID.h"
#include "Milky/Core/Timestep.h"
#include "Milky/Renderer/EditorCamera.h"

#include <entt.hpp>

class b2World;

namespace Milky {

	class Entity;

	class Scene {
		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	public:
		Scene();
		~Scene();

		static Ref<Scene> Copy(const Ref<Scene> other);

		Entity CreateEntity(UUID uuid, const std::string& name = std::string());
		Entity CreateEntity(const std::string& name = std::string());

		void DestroyEntity(Entity entity);
		void DestroyEntities(const std::vector<Entity> entities);

		void DuplicateEntity(Entity entity);
		void DuplicateEntities(const std::vector<Entity> entities);

		Entity GetEntity(UUID uuid);
		const std::vector<Entity> GetEntities(const std::vector<UUID> uuids);

		void OnRuntimeStart();
		void OnRuntimeStop();

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

		b2World* m_PhysicsWorld = nullptr;
	};

}
