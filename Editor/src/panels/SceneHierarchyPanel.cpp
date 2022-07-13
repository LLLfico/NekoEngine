#include "pch.h"
#include "SceneHierarchyPanel.h"

#include "core/Core.h"
#include "core/Log.h"
#include "world/Scene.h"
#include "world/Component.h"

#include <imgui/imgui.h>

namespace Neko {

	SceneHierarchyPanel::SceneHierarchyPanel(const std::shared_ptr<Scene>& scene) {
		SetContext(scene);
	}

	void SceneHierarchyPanel::SetContext(const std::shared_ptr<Scene>& scene) {
		m_context = scene;
	}

	void SceneHierarchyPanel::OnImGuiRender() {
		ImGui::Begin("Scene Hierarchy");

		m_context->m_registry.each([&](auto entityId) {
			Entity entity{ entityId, m_context.get() };
			DrawEntityMode(entity);
		});

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityMode(Entity entity) {
		auto& tag = entity.GetComponent<TagComponent>().tag;

		ImGuiTreeNodeFlags flags = ((m_selectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked()) {
			m_selectionContext = entity;
		}

		if (opened) {
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx((void*)9817237, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}
	}

}