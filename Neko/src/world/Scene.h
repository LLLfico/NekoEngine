#pragma once

#include "core/TimeStep.h"

#include <entt.hpp>

namespace Neko {

	class Entity;
	class NEKO_API Scene {
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = "");

		void OnUpdate(TimeStep dt);
	private:
		friend class Entity;
		entt::registry m_registry;
	};

}