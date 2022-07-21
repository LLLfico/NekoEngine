#pragma once

#include "Scene.h"
#include "Component.h"
#include "core/UUID.h"

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
			T& component =  m_scene->m_registry.emplace<T>(m_handle, std::forward<Args>(args)...);
			m_scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T, typename ...Args>
		T& AddorReplaceComponent(Args&&... args) {
			T& component = m_scene->m_registry.emplace_or_replace<T>(m_handle, std::forward<Args>(args)...);
			m_scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T>
		T& GetComponent() {
			NEKO_CORE_ASSERT(HasComponent<T>(), "Entity doesn't has component!");
			return m_scene->m_registry.get<T>(m_handle);
		}

		template<typename T>
		bool HasComponent() {
			return m_scene->m_registry.all_of<T>(m_handle);
		}

		template<typename T>
		void RemoveComponent() {
			NEKO_CORE_ASSERT(HasComponent<T>(), "Entity doesn't has component!");
			m_scene->m_registry.remove<T>(m_handle);
		}

		operator bool() const { return m_handle != entt::null; }
		operator uint32_t() const { return (uint32_t)m_handle; };
		operator entt::entity() const { return m_handle; }

		UUID GetUUID() { return GetComponent<IDComponent>().id; }
		const std::string& GetName() { return GetComponent<TagComponent>().tag; }

		bool operator==(const Entity& other) const {
			return m_handle == other.m_handle && m_scene == other.m_scene;
		}
		bool operator!=(const Entity& other) const {
			return !(*this == other);
		}
	private:
		entt::entity m_handle{ entt::null };
		Scene* m_scene = nullptr;

	};

}