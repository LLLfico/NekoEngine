#pragma once

#include "core/TimeStep.h"
#include "core/renderer/EditorCamera.h"

#include <entt.hpp>

class b2World;
namespace Neko {

	class UUID;
	class Entity;
	class NEKO_API Scene {
		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	public:
		Scene();
		~Scene();

		static std::shared_ptr<Scene> CopyFrom(std::shared_ptr<Scene> other);

		Entity CreateEntity(const std::string& name = "");
		Entity CreateEntityWithUUID(UUID id, const std::string& name = "");
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdateRuntime(TimeStep dt);
		void OnUpdateEditor(TimeStep dt, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);

		void DuplicateEntity(Entity entity);

		Entity GetPrimaryCameraEntity();

		template<typename... Components>
		auto GetAllEntityWithComponent() {
			return m_registry.view<Components...>();
		}
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_registry;
		uint32_t m_viewportWidth = 0, m_viewportHeight = 0;

		std::shared_ptr<b2World> m_physicsWorld = nullptr;
	};

}