#include "pch.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Scene.h"
#include "Component.h"

#include <yaml-cpp/yaml.h>
#include <fstream>

namespace YAML {

	template <>
	struct convert<glm::vec2> {
		static Node encode(const glm::vec2& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}
		static bool decode(const Node& node, glm::vec2& rhs) {
			if (!node.IsSequence() || node.size() != 2) {
				return false;
			}
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template <>
	struct convert<glm::vec3> {
		static Node encode(const glm::vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}
		static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3) {
				return false;
			}
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template <>
	struct convert<glm::vec4> {
		static Node encode(const glm::vec4& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}
		static bool decode(const Node& node, glm::vec4& rhs) {
			if (!node.IsSequence() || node.size() != 4) {
				return false;
			}
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}


namespace Neko {

	static std::unordered_map<Rigidbody2DComponent::BodyType, std::string> s_rigidBodyTypeStringMap = {
		{Rigidbody2DComponent::BodyType::Static, "Static"},
		{Rigidbody2DComponent::BodyType::Dynamic, "Dynamic"},
		{Rigidbody2DComponent::BodyType::Kinematic, "Kinematic"},
	};
	static std::unordered_map<std::string, Rigidbody2DComponent::BodyType> s_stringRigidBodyTypeMap = {
		{"Static", Rigidbody2DComponent::BodyType::Static},
		{"Dynamic", Rigidbody2DComponent::BodyType::Dynamic},
		{"Kinematic", Rigidbody2DComponent::BodyType::Kinematic},
	};

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	SceneSerializer::SceneSerializer(const std::shared_ptr<Scene>& scene) : m_scene(scene) {
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity) {
		NEKO_CORE_ASSERT(entity.HasComponent<IDComponent>());
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		if (entity.HasComponent<TagComponent>()) {
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;

			auto& tag = entity.GetComponent<TagComponent>().tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>()) {
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			auto& transform = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << transform.translation;
			out << YAML::Key << "Rotation" << YAML::Value << transform.rotation;
			out << YAML::Key << "Scale" << YAML::Value << transform.scale;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>()) {
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.camera;
			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFov" << YAML::Value << camera.GetPerspectiveFov();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap;

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.fixedAspectRatio;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<SpriteRendererComponent>()) {
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;

			auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.color;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<CircleRendererComponent>()) {
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap; // CircleRendererComponent

			auto& circleRendererComponent = entity.GetComponent<CircleRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << circleRendererComponent.color;
			out << YAML::Key << "Thickness" << YAML::Value << circleRendererComponent.thickness;
			out << YAML::Key << "Fade" << YAML::Value << circleRendererComponent.fade;

			out << YAML::EndMap; // CircleRendererComponent
		}

		if (entity.HasComponent<Rigidbody2DComponent>()) {
			out << YAML::Key << "Rigidbody2DComponent";
			out << YAML::BeginMap; // Rigidbody2DComponent

			auto& rb2dComponent = entity.GetComponent<Rigidbody2DComponent>();
			out << YAML::Key << "BodyType" << YAML::Value << s_rigidBodyTypeStringMap[rb2dComponent.type];
			out << YAML::Key << "FixedRotation" << YAML::Value << rb2dComponent.fixedRotation;

			out << YAML::EndMap;
		}
		if (entity.HasComponent<BoxCollider2DComponent>()) {
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap; // BoxCollider2DComponent

			auto& bc2dComponent = entity.GetComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << bc2dComponent.offset;
			out << YAML::Key << "Size" << YAML::Value << bc2dComponent.size;
			out << YAML::Key << "Density" << YAML::Value << bc2dComponent.density;
			out << YAML::Key << "Friction" << YAML::Value << bc2dComponent.friction;
			out << YAML::Key << "Restitution" << YAML::Value << bc2dComponent.restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc2dComponent.restitutionThreshold;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<CircleCollider2DComponent>()) {
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap; // CircleCollider2DComponent

			auto& cc2dComponent = entity.GetComponent<CircleCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << cc2dComponent.offset;
			out << YAML::Key << "Radius" << YAML::Value << cc2dComponent.radius;
			out << YAML::Key << "Density" << YAML::Value << cc2dComponent.density;
			out << YAML::Key << "Friction" << YAML::Value << cc2dComponent.friction;
			out << YAML::Key << "Restitution" << YAML::Value << cc2dComponent.restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << cc2dComponent.restitutionThreshold;

			out << YAML::EndMap; // CircleCollider2DComponent
		}

		if (entity.HasComponent<MeshComponent>()) {
			out << YAML::Key << "MeshComponent";
			out << YAML::BeginMap; // 

			auto& meshComponent = entity.GetComponent<MeshComponent>();
			out << YAML::Key << "FilePath" << YAML::Value << meshComponent.mesh->GetPath();

			out << YAML::EndMap; // MeshComponent
		}
		out << YAML::EndMap;
	}

	void SceneSerializer::Serialize(const std::string& filePath) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_scene->m_registry.each([&](auto entityId) {
			Entity entity = { entityId, m_scene.get() };
			if (!entity)
				return;
			SerializeEntity(out, entity);
		});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filePath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::string& filePath) {
		NEKO_CORE_ASSERT(false, "");
	}

	bool SceneSerializer::Deserialize(const std::string& filePath) {
		YAML::Node data;
		try {
			data = YAML::LoadFile(filePath);
		}
		catch (YAML::ParserException e) {
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		NEKO_CORE_TRACE("Deserialize scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (!entities)
			return true;

		for (auto entity : entities) {
			uint64_t uuid = entity["Entity"].as<uint64_t>();

			std::string name;
			auto tagComponent = entity["TagComponent"];
			if (tagComponent) {
				name = tagComponent["Tag"].as<std::string>();
			}
			NEKO_CORE_TRACE("Deserialize entity with Id = {0}, name = {1}", uuid, name);

			Entity deserializedEntity = m_scene->CreateEntityWithUUID(uuid, name);

			auto transformComponent = entity["TransformComponent"];
			if (transformComponent) {
				auto& tc = deserializedEntity.GetComponent<TransformComponent>();
				tc.translation = transformComponent["Translation"].as<glm::vec3>();
				tc.rotation = transformComponent["Rotation"].as<glm::vec3>();
				tc.scale = transformComponent["Scale"].as<glm::vec3>();
			}

			auto cameraComponent = entity["CameraComponent"];
			if (cameraComponent) {
				auto& cc = deserializedEntity.AddComponent<CameraComponent>();
				auto& camera = cameraComponent["Camera"];

				cc.camera.SetProjectionType(SceneCamera::ProjectionType(camera["ProjectionType"].as<int>()));
				cc.camera.SetPerspectiveFov(camera["PerspectiveFov"].as<float>());
				cc.camera.SetPerspectiveNearClip(camera["PerspectiveNear"].as<float>());
				cc.camera.SetPerspectiveFarClip(camera["PerspectiveFar"].as<float>());

				cc.camera.SetOrthographicSize(camera["OrthographicSize"].as<float>());
				cc.camera.SetOrthographicNearClip(camera["OrthographicNear"].as<float>());
				cc.camera.SetOrthographicFarClip(camera["OrthographicFar"].as<float>());

				cc.primary = cameraComponent["Primary"].as<bool>();
				cc.fixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
			}

			auto spriteRendererComponent = entity["SpriteRendererComponent"];
			if (spriteRendererComponent) {
				auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
				src.color = spriteRendererComponent["Color"].as<glm::vec4>();
			}

			auto circleRendererComponent = entity["CircleRendererComponent"];
			if (circleRendererComponent) {
				auto& crc = deserializedEntity.AddComponent<CircleRendererComponent>();
				crc.color = circleRendererComponent["Color"].as<glm::vec4>();
				crc.thickness = circleRendererComponent["Thickness"].as<float>();
				crc.fade = circleRendererComponent["Fade"].as<float>();
			}

			auto rigidbody2DComponent = entity["Rigidbody2DComponent"];
			if (rigidbody2DComponent) {
				auto& rb2d = deserializedEntity.AddComponent<Rigidbody2DComponent>();
				rb2d.type = s_stringRigidBodyTypeMap[rigidbody2DComponent["BodyType"].as<std::string>()];
				rb2d.fixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
			}

			auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
			if (boxCollider2DComponent) {
				auto& bc2d = deserializedEntity.AddComponent<BoxCollider2DComponent>();
				bc2d.offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
				bc2d.size = boxCollider2DComponent["Size"].as<glm::vec2>();
				bc2d.density = boxCollider2DComponent["Density"].as<float>();
				bc2d.friction = boxCollider2DComponent["Friction"].as<float>();
				bc2d.restitution = boxCollider2DComponent["Restitution"].as<float>();
				bc2d.restitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
			}

			auto circleCollider2DComponent = entity["CircleCollider2DComponent"];
			if (circleCollider2DComponent) {
				auto& cc2d = deserializedEntity.AddComponent<CircleCollider2DComponent>();
				cc2d.offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
				cc2d.radius = circleCollider2DComponent["Radius"].as<float>();
				cc2d.density = circleCollider2DComponent["Density"].as<float>();
				cc2d.friction = circleCollider2DComponent["Friction"].as<float>();
				cc2d.restitution = circleCollider2DComponent["Restitution"].as<float>();
				cc2d.restitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<float>();
			}

			auto meshComponent = entity["MeshComponent"];
			if (meshComponent) {
				auto path = meshComponent["FilePath"].as<std::string>();
				auto& mc = deserializedEntity.AddComponent<MeshComponent>(path);
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filePath) {
		NEKO_CORE_ASSERT(false, "");
		return false;
	}

}