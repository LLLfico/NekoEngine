#pragma once

#include "Scene.h"
#include <entt.hpp>

namespace Neko {

	class Entity {
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity&) = default;

		template<typename T, typename ...Args>
		T& AddComponent(Args&&... args) {
			NEKO_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			return m_scene->m_registry.emplace<T>(m_handle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent() {
			NEKO_CORE_ASSERT(HasComponent<T>(), "Entity doesn't has component!");
			return m_scene->m_registry.get<T>(m_handle);
		}

		template<typename T>
		bool HasComponent() {
			return m_scene->m_registry.any_of<T>(m_handle);
		}

		template<typename T>
		void RemoveComponent() {
			NEKO_CORE_ASSERT(HasComponent<T>(), "Entity doesn't has component!");
			m_scene->m_registry.remove<T>(m_handle);
		}

		operator bool() const { return m_handle != entt::entity{0}; }
		operator uint32_t() const { return (uint32_t)m_handle; };

		bool operator==(const Entity& other) const {
			return m_handle == other.m_handle && m_scene == other.m_scene;
		}
		bool operator!=(const Entity& other) const {
			return !(*this == other);
		}
	private:
		entt::entity m_handle{ 0 };
		Scene* m_scene = nullptr;

	};

}