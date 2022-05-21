#include "mlpch.h"
#include "Scene.h"

#include "Components.h"
#include "Milky/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

#include "Entity.h"

namespace Milky {

	Scene::Scene()
	{}

	Scene::~Scene()
	{}

	Entity Scene::CreateEmptyEntity()
	{
		return { m_Registry.create(), this };
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = CreateEmptyEntity();
		entity.AddComponent<TagComponent>(name.empty() ? "Unnamed Entity" : name);
		entity.AddComponent<TransformComponent>();
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnUpdate(Timestep ts)
	{
		// Update entity scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](entt::entity entity, NativeScriptComponent& nsc)
			{
				// TODO: Move to Scene::OnScenePlay
				if (!nsc.Instance)
				{
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->m_Entity = Entity{ entity, this };
					nsc.Instance->OnCreate();
				}

				nsc.Instance->OnUpdate(ts);
				// TODO: Destroy scripts in Scene::OnSceneStop
			});
		}

		// Render sprites
		
		Entity mainCameraEntity = GetPrimaryCameraEntity();
		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;
		if (mainCameraEntity)
		{
			mainCamera = &mainCameraEntity.GetComponent<CameraComponent>().Camera;
			cameraTransform = &mainCameraEntity.GetComponent<TransformComponent>().GetTransform();
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(mainCamera->GetProjection(), *cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			int squares = 0;
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
			}

			Renderer2D::EndScene();
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

	size_t Scene::GetNumEntites()
	{
		return m_Registry.size();
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize all entities with CameraComponent AND have FixedAspectRatio set to false.
		auto view = m_Registry.view<CameraComponent, TransformComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
			{
				cameraComponent.Camera.SetViewportSize(width, height);
			}
		}
	}

	template<typename Comp>
	void Scene::OnComponentAdded(Entity entity, Comp& component)
	{
		static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{}

}
