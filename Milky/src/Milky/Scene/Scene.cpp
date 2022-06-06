#include "mlpch.h"
#include "Scene.h"

#include "Components/Components.h"
#include "Milky/Renderer/Renderer2D.h"
#include "ScriptableEntity.h"

#include <glm/glm.hpp>

#include "Entity.h"

#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>

namespace Milky {

	static b2BodyType MilkyRigidBodyTypeToBox2D(RigidBody2DComponent::BodyType bodyType)
	{
		switch(bodyType)
		{
		case RigidBody2DComponent::BodyType::Static: return b2_staticBody;
		case RigidBody2DComponent::BodyType::Dynamic: return b2_dynamicBody;
		case RigidBody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
		}

		ML_CORE_ASSERT(false, "Unknown Box2D body type!");
		return b2_staticBody;
	}

	Scene::Scene()
	{}

	Scene::~Scene()
	{}

	template<typename Comp>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		auto view = src.view<Comp>();
		for (auto e : view)
		{
			UUID uuid = src.get<IDComponent>(e).ID;
			ML_CORE_ASSERT(enttMap.find(uuid) != enttMap.end(), "Entity does not exist in copy map!");
			entt::entity dstEnttID = enttMap.at(uuid);

			auto& component = src.get<Comp>(e);
			dst.emplace_or_replace<Comp>(dstEnttID, component);
		}
	}

	template<typename Comp>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		if (src.HasComponent<Comp>())
			dst.AddOrReplaceComponent<Comp>(src.GetComponent<Comp>());
	}

	Ref<Scene> Scene::Copy(const Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		std::unordered_map<UUID, entt::entity> enttMap;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const std::string name = srcSceneRegistry.get<TagComponent>(e).Tag;
			enttMap[uuid] = newScene->CreateEntity(uuid, name);
		}

		CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<SpriteRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CircleRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<RigidBody2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<BoxCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CircleCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

		return newScene;
	}

	Entity Scene::CreateEntity(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TagComponent>(name.empty() ? "Unnamed Entity" : name);
		entity.AddComponent<TransformComponent>();
		return entity;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntity(UUID(), name);
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::DestroyEntities(const std::vector<Entity>& entities)
	{
		for(Entity entity : entities)
			m_Registry.destroy(entity);
	}

	void Scene::DuplicateEntity(Entity entity)
	{
		Entity newEntity = CreateEntity(entity.GetName());
		CopyComponentIfExists<TransformComponent>(newEntity, entity);
		CopyComponentIfExists<SpriteRendererComponent>(newEntity, entity);
		CopyComponentIfExists<CircleRendererComponent>(newEntity, entity);
		CopyComponentIfExists<CameraComponent>(newEntity, entity);
		CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);
		CopyComponentIfExists<RigidBody2DComponent>(newEntity, entity);
		CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
		CopyComponentIfExists<CircleCollider2DComponent>(newEntity, entity);
	}

	void Scene::DuplicateEntities(const std::vector<Entity>& entities)
	{
		for (Entity entity : entities)
			DuplicateEntity(entity);
	}

	Entity Scene::GetEntity(UUID uuid)
	{
		if (!uuid)
			return {};
		auto idView = m_Registry.view<IDComponent>();
		for (auto e : idView)
			if (m_Registry.get<IDComponent>(e).ID == uuid)
				return { e, this };
		return {};
	}

	const std::vector<Entity> Scene::GetEntities(const std::vector<UUID>& uuids)
	{
		std::vector<Entity> result;
		if (uuids.empty())
			return result;
		for (UUID uuid : uuids)
		{
			Entity entity = GetEntity(uuid);
			if (entity)
				result.push_back(entity);
		}
		return result;
	}

	void Scene::OnRuntimeStart()
	{
		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });
		auto view = m_Registry.view<RigidBody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& tc = entity.GetTransform();
			auto& rb2dc = entity.GetComponent<RigidBody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = MilkyRigidBodyTypeToBox2D(rb2dc.Type);
			bodyDef.position.Set(tc.Translation.x, tc.Translation.y);
			bodyDef.angle = tc.Rotation.z;
			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2dc.FixedRotation);
			rb2dc.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2dc = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2dc.Size.x * tc.Scale.x, bc2dc.Size.y * tc.Scale.y, { bc2dc.Offset.x, bc2dc.Offset.y }, 0.0f);
				
				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2dc.Density;
				fixtureDef.friction = bc2dc.Friction;
				fixtureDef.restitution = bc2dc.Restitution;
				fixtureDef.restitutionThreshold = bc2dc.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}

			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				auto& cc2dc = entity.GetComponent<CircleCollider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2dc.Offset.x, cc2dc.Offset.y);
				circleShape.m_radius = (tc.Scale.x + tc.Scale.y) * 0.5f * cc2dc.Radius;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2dc.Density;
				fixtureDef.friction = cc2dc.Friction;
				fixtureDef.restitution = cc2dc.Restitution;
				fixtureDef.restitutionThreshold = cc2dc.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		// Scripts
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

		// Physics
		{
			const int32_t velocityIterations = 6;
			const int32_t positionIterations = 2;
			m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

			auto view = m_Registry.view<RigidBody2DComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				auto& tc = entity.GetTransform();
				auto& rb2dc = entity.GetComponent<RigidBody2DComponent>();
				b2Body* body = (b2Body*)rb2dc.RuntimeBody;
				const auto& position = body->GetPosition();
				tc.Translation.x = position.x;
				tc.Translation.y = position.y;
				tc.Rotation.z = body->GetAngle();
			}
		}

		// Render 2D
		Entity mainCameraEntity = GetPrimaryCameraEntity();
		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;
		if (mainCameraEntity)
		{
			mainCamera = &mainCameraEntity.GetComponent<CameraComponent>().Camera;
			cameraTransform = &mainCameraEntity.GetTransform().GetTransform();
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(mainCamera->GetProjection(), *cameraTransform);

			// Sprites
			{
				auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);
					Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
				}
			}

			// Circles
			{
				auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
					Renderer2D::DrawCircle(transform.GetTransform(), circle, (int)entity);
				}
			}
			Renderer2D::EndScene();
		}
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		// Sprites
		{
			auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			}
		}

		// Circles
		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer2D::DrawCircle(transform.GetTransform(), circle, (int)entity);
			}
		}

		Renderer2D::EndScene();
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

	const std::vector<Entity> Scene::GetAllEntities()
	{
		std::vector<Entity> entities;
		auto idView = m_Registry.view<IDComponent>();
		for (auto e : idView)
			entities.push_back({ e, this });
		return entities;
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
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{}

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
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
	{}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{}

	template<>
	void Scene::OnComponentAdded<RigidBody2DComponent>(Entity entity, RigidBody2DComponent& component)
	{}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{}

	template<>
	void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
	{}

}
