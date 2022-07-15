#pragma once
#include "world/Entity.h"

namespace Neko {

	class Scene;
	class SceneHierarchyPanel {
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const std::shared_ptr<Scene>& scene);

		void SetContext(const std::shared_ptr<Scene>& scene);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_selectionContext; }
	private:
		void DrawEntityMode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		std::shared_ptr<Scene> m_context;
		Entity m_selectionContext;
	};

}


