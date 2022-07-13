#pragma once

#include "core/TimeStep.h"

#include <entt.hpp>

namespace Neko {

	class Entity;
	class NEKO_API Scene {
		friend class Entity;
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = "");

		void OnUpdate(TimeStep dt);
		void OnViewportResize(uint32_t width, uint32_t height);
	private:
		entt::registry m_registry;
		uint32_t m_viewportWidth = 0, m_viewportHeight = 0;
	};

}