#include "mlpch.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components/Components.h"

#include <filesystem>

#include <yaml-cpp/yaml.h>

namespace YAML {

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	Emitter& operator<<(Emitter& out, const glm::vec2& v)
	{
		out << Flow;
		out << BeginSeq << v.x << v.y << EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::vec3& v)
	{
		out << Flow;
		out << BeginSeq << v.x << v.y << v.z << EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::vec4& v)
	{
		out << Flow;
		out << BeginSeq << v.x << v.y << v.z << v.w << EndSeq;
		return out;
	}

}

namespace Milky {

	static const std::string RigidBody2DBodyTypeToString(const RigidBody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
		case RigidBody2DComponent::BodyType::Static: return "Static";
		case RigidBody2DComponent::BodyType::Dynamic: return "Dynamic";
		case RigidBody2DComponent::BodyType::Kinematic: return "Kinematic";
		}

		ML_CORE_ASSERT(false, "Unknown Box2D body type!");
		return "Unknown";
	}

	static RigidBody2DComponent::BodyType RigidBody2DStringToBodyType(const std::string& string)
	{
		if (string == "Static") return RigidBody2DComponent::BodyType::Static;
		if (string == "Dynamic") return RigidBody2DComponent::BodyType::Dynamic;
		if (string == "Kinematic") return RigidBody2DComponent::BodyType::Kinematic;
		ML_CORE_ASSERT(false, "Unknown Box2D body type!");
		return RigidBody2DComponent::BodyType::Static;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{}

	template<typename Comp>
	static void SerializeComponent(YAML::Emitter& out, Entity entity)
	{ static_assert(false); }

	template<>
	static void SerializeComponent<TagComponent>(YAML::Emitter& out, Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // TagComponent

			TagComponent& tc = entity.GetComponent<TagComponent>();
			out << YAML::Key << "Tag" << YAML::Value << tc.Tag;

			out << YAML::EndMap; // TagComponent
		}
	}

	template<>
	static void SerializeComponent<TransformComponent>(YAML::Emitter& out, Entity entity)
	{
		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // TransformComponent

			TransformComponent& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

			out << YAML::EndMap; // TransformComponent
		}
	}

	template<>
	static void SerializeComponent<CameraComponent>(YAML::Emitter& out, Entity entity)
	{
		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent

			CameraComponent& cc = entity.GetComponent<CameraComponent>();
			auto& camera = cc.Camera;

			out << YAML::Key << "Camera";
			out << YAML::BeginMap; // Camera
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();

			out << YAML::Key << "PerspectiveVerticalFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();

			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap; // Camera

			out << YAML::Key << "Primary" << YAML::Value << cc.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cc.FixedAspectRatio;

			out << YAML::EndMap; // CameraComponent
		}
	}

	template<>
	static void SerializeComponent<SpriteRendererComponent>(YAML::Emitter& out, Entity entity)
	{
		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // SpriteRendererComponent

			SpriteRendererComponent& src = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << src.Color;
			// TODO: Texture serialization
			// out << YAML::Key << "Textire" << YAML::Value << src.Texture->Path();
			out << YAML::Key << "TilingFactor" << YAML::Value << src.TilingFactor;
			out << YAML::EndMap; // SpriteRendererComponent
		}
	}

	template<>
	static void SerializeComponent<RigidBody2DComponent>(YAML::Emitter& out, Entity entity)
	{
		if (entity.HasComponent<RigidBody2DComponent>())
		{
			out << YAML::Key << "RigidBody2DComponent";
			out << YAML::BeginMap; // RigidBody2DComponent

			RigidBody2DComponent& rb2dc = entity.GetComponent<RigidBody2DComponent>();
			out << YAML::Key << "Type" << YAML::Value << RigidBody2DBodyTypeToString(rb2dc.Type);
			out << YAML::Key << "FixedRotation" << YAML::Value << rb2dc.FixedRotation;

			out << YAML::EndMap; // RigidBody2DComponent
		}
	}

	template<>
	static void SerializeComponent<BoxCollider2DComponent>(YAML::Emitter& out, Entity entity)
	{
		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap; // BoxCollider2DComponent

			BoxCollider2DComponent& bc2dc = entity.GetComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << bc2dc.Offset;
			out << YAML::Key << "Size" << YAML::Value << bc2dc.Size;
			out << YAML::Key << "Density" << YAML::Value << bc2dc.Density;
			out << YAML::Key << "Friction" << YAML::Value << bc2dc.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << bc2dc.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc2dc.RestitutionThreshold;

			out << YAML::EndMap; // BoxCollider2DComponent
		}
	}

	template<typename Comp>
	static bool DeserializeComponent(YAML::Node entityData, Entity entity)
	{ static_assert(false); }

	template<>
	static bool DeserializeComponent<TagComponent>(YAML::Node entityData, Entity entity)
	{
		YAML::Node tcData = entityData["TagComponent"];
		if (tcData)
		{
			TagComponent& tc = entity.GetComponent<TagComponent>();
			tc.Tag = tcData["Tag"].as<std::string>();
			return true;
		}
		return false;
	}

	template<>
	static bool DeserializeComponent<TransformComponent>(YAML::Node entityData, Entity entity)
	{
		YAML::Node tcData = entityData["TransformComponent"];
		if (tcData)
		{
			TransformComponent& tc = entity.GetComponent<TransformComponent>();
			tc.Translation = tcData["Translation"].as<glm::vec3>();
			tc.Rotation = tcData["Rotation"].as<glm::vec3>();
			tc.Scale = tcData["Scale"].as<glm::vec3>();
			return true;
		}
		return false;
	}

	template<>
	static bool DeserializeComponent<CameraComponent>(YAML::Node entityData, Entity entity)
	{
		YAML::Node ccData = entityData["CameraComponent"];
		if (ccData)
		{
			CameraComponent& cc = entity.AddComponent<CameraComponent>();

			YAML::Node& camData = ccData["Camera"];
			cc.Camera.SetProjectionType((SceneCamera::ProjectionType)camData["ProjectionType"].as<int>());
			
			cc.Camera.SetPerspectiveVerticalFOV(camData["PerspectiveVerticalFOV"].as<float>());
			cc.Camera.SetPerspectiveNearClip(camData["PerspectiveNear"].as<float>());
			cc.Camera.SetPerspectiveFarClip(camData["PerspectiveFar"].as<float>());

			cc.Camera.SetOrthographicSize(camData["OrthographicSize"].as<float>());
			cc.Camera.SetOrthographicNearClip(camData["OrthographicNear"].as<float>());
			cc.Camera.SetOrthographicFarClip(camData["OrthographicFar"].as<float>());

			cc.Primary = ccData["Primary"].as<bool>();
			cc.FixedAspectRatio = ccData["FixedAspectRatio"].as<bool>();
			return true;
		}
		return false;
	}


	template<>
	static bool DeserializeComponent<SpriteRendererComponent>(YAML::Node entityData, Entity entity)
	{
		YAML::Node& srcData = entityData["SpriteRendererComponent"];
		if (srcData)
		{
			SpriteRendererComponent& src = entity.AddComponent<SpriteRendererComponent>();
			src.Color = srcData["Color"].as<glm::vec4>();
			// TODO: Texture deserialization
			// src.Texture = Texture2D::Create(srcData["Texture"].as<std::string>());
			src.TilingFactor = srcData["TilingFactor"].as<float>();
			return true;
		}
		return false;
	}

	template<>
	static bool DeserializeComponent<RigidBody2DComponent>(YAML::Node entityData, Entity entity)
	{
		YAML::Node& rb2dcData = entityData["RigidBody2DComponent"];
		if (rb2dcData)
		{
			RigidBody2DComponent& rb2dc = entity.AddComponent<RigidBody2DComponent>();
			rb2dc.Type = RigidBody2DStringToBodyType(rb2dcData["Type"].as<std::string>());
			rb2dc.FixedRotation = rb2dcData["FixedRotation"].as<bool>();
			return true;
		}
		return false;
	}

	template<>
	static bool DeserializeComponent<BoxCollider2DComponent>(YAML::Node entityData, Entity entity)
	{
		YAML::Node& bc2dcData = entityData["BoxCollider2DComponent"];
		if (bc2dcData)
		{
			BoxCollider2DComponent& bc2dc = entity.AddComponent<BoxCollider2DComponent>();
			bc2dc.Offset = bc2dcData["Offset"].as<glm::vec2>();
			bc2dc.Size = bc2dcData["Size"].as<glm::vec2>();
			bc2dc.Density = bc2dcData["Density"].as<float>();
			bc2dc.Friction = bc2dcData["Friction"].as<float>();
			bc2dc.Restitution = bc2dcData["Restitution"].as<float>();
			bc2dc.RestitutionThreshold = bc2dcData["RestitutionThreshold"].as<float>();
			return true;
		}
		return false;
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID(); // TODO: Entity ID

		SerializeComponent<TagComponent>(out, entity);
		SerializeComponent<TransformComponent>(out, entity);
		SerializeComponent<CameraComponent>(out, entity);
		SerializeComponent<SpriteRendererComponent>(out, entity);
		SerializeComponent<RigidBody2DComponent>(out, entity);
		SerializeComponent<BoxCollider2DComponent>(out, entity);

		out << YAML::EndMap; // Entity
	}

	static bool DeserializeEntity(YAML::Node entityData, Entity entity)
	{
		bool result = true;
		result &= DeserializeComponent<TagComponent>(entityData, entity);
		ML_CORE_TRACE("Deserializing entity '{0}' ({1})", entity.GetName(), entity.GetUUID());
		DeserializeComponent<TransformComponent>(entityData, entity);
		DeserializeComponent<CameraComponent>(entityData, entity);
		DeserializeComponent<SpriteRendererComponent>(entityData, entity);
		DeserializeComponent<RigidBody2DComponent>(entityData, entity);
		DeserializeComponent<BoxCollider2DComponent>(entityData, entity);

		return result;
	}

	void SceneSerializer::Serialize(const std::string & filepath)
	{
		YAML::Emitter out;

		out << YAML::BeginMap; // Scene
		out << YAML::Key << "Scene" << YAML::Value << "Untitled"; // TODO: Named Scene
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq; // Entities
		m_Scene->m_Registry.each([&](auto entityID)
		{
			Entity entity = { entityID, m_Scene.get() };
			if (!entity)
				return;

			SerializeEntity(out, entity);
		});
		out << YAML::EndSeq; // Entities
		out << YAML::EndMap; // Scene
		
		std::string dir;
		const size_t lastSlashIdx = filepath.rfind('/');
		if (std::string::npos != lastSlashIdx)
		{
			dir = filepath.substr(0, lastSlashIdx);
			if (!std::filesystem::exists(dir))
				std::filesystem::create_directories(dir);
		}
		ML_CORE_INFO("Saving scene file: '{0}'", filepath);
		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::string & filepath)
	{}

	bool SceneSerializer::Deserialize(const std::string & filepath)
	{
		ML_CORE_INFO("Loading scene file: '{0}'", filepath);
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (YAML::ParserException e)
		{
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		ML_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		YAML::Node entityArrayData = data["Entities"];
		if (entityArrayData)
		{
			for (YAML::Node entityData : entityArrayData)
			{
				uint64_t uuid = entityData["Entity"].as<uint64_t>();
				Entity entity = m_Scene->CreateEntity(uuid);
				if (!DeserializeEntity(entityData, entity))
					m_Scene->DestroyEntity(entity);
			}
		}
		else
		{
			ML_CORE_WARN("Scene has no Entity data!");
			return false;
		}
		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string & filepath)
	{
		ML_CORE_ASSERT(false, "SceneSerializer::DeserializeRuntime Not yet implemented!");
		return false;
	}

}
