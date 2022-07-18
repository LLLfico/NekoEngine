#pragma once
#include <Neko.h>

#include "panels/SceneHierarchyPanel.h"
#include "panels/ContentBrowserPanel.h"

namespace Neko {

	class EditorLayer : public Layer {
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(TimeStep dt) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& e) override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonEvent& e);

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();

		void OnScenePlay();
		void OnSceneStop();

		void UI_Toolbar();

	private:
		CameraController m_cameraController;

		std::shared_ptr<VertexArray> m_squareVAO;
		std::shared_ptr<Neko::Shader> m_flatColorShader;

		std::shared_ptr<Texture2D> m_texture;

		std::shared_ptr<Neko::FrameBuffer> m_framebuffer;

		std::shared_ptr<Scene> m_scene;
		Entity m_squareEntity;
		Entity m_cametaEntity;
		Entity m_secondCamera;

		Entity m_hoveredEntity = {entt::null, nullptr};

		bool m_primaryCamera = true;

		EditorCamera m_editorCamera;

		bool m_viewportFocused = false, m_viewportHovered = false;
		glm::vec2 m_viewportSize = { 0.0f, 0.0f };
		glm::vec2 m_viewportBounds[2];

		glm::vec4 m_squareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

		int m_gizmoType = -1;

		enum class SceneState {
			Edit = 0, 
			Play = 1,
		};
		SceneState m_sceneState = SceneState::Edit;

		SceneHierarchyPanel m_sceneHierarchyPanel;
		ContentBrowserPanel m_contentBrowserPanel;

		std::shared_ptr<Texture2D> m_playIcon;
		std::shared_ptr<Texture2D> m_stopIcon;
	};

}