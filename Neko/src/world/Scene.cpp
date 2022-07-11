#include "pch.h"
#include "Scene.h"

#include "Entity.h"
#include "Component.h"
#include "core/Renderer/Renderer2D.h"

namespace Neko {

	static void DoMath(const glm::mat4& transform) {

	}

	static void OnTransformConstruct(entt::registry& registry, entt::entity entity) {

	}

	Scene::Scene() {
		struct MeshComponent;

		entt::entity entity = m_registry.create();
		m_registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));
		m_registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>();

		if (m_registry.any_of<TransformComponent>(entity)) {
			auto& transform = m_registry.get<TransformComponent>(entity);
		}

		auto view = m_registry.view<TransformComponent>();
		for (auto entity : view) {
			auto& transform = view.get<TransformComponent>(entity);
		}
	}

	Scene::~Scene() {
	}

	Entity Scene::CreateEntity(const std::string& name) {
		Entity entity = { m_registry.create(), this };
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.tag = name.empty() ? "Entity" : name;
		return entity;
	}

	

	void Scene::OnUpdate(TimeStep dt) {
		auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group) {
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			Renderer2D::DrawQuad(transform, sprite.color);
		}
	}

}