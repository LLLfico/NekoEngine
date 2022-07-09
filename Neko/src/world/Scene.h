#pragma once

#include "core/TimeStep.h"

#include <entt.hpp>

namespace Neko {

	class NEKO_API Scene {
	public:
		Scene();
		~Scene();

		entt::entity CreateEntity();

		entt::registry& GetRegistry() { return m_registry; }

		void OnUpdate(TimeStep dt);
	private:
		entt::registry m_registry;
	};

}