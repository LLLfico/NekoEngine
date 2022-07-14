#pragma once

#include "core/TimeStep.h"

#include <entt.hpp>

namespace Neko {

	class Entity;
	class NEKO_API Scene {
		friend class Entity;
		friend class SceneHierarchyPanel;
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = "");
		void DestroyEntity(Entity entity);

		void OnUpdate(TimeStep dt);
		void OnViewportResize(uint32_t width, uint32_t height);
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_registry;
		uint32_t m_viewportWidth = 0, m_viewportHeight = 0;
	};

}