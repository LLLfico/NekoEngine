#pragma once
#include <Neko.h>

#include "panels/SceneHierarchyPanel.h"

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

		void NewScene();
		void OpenScene();
		void SaveScene();

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

		bool m_primaryCamera = true;

		bool m_viewportFocused = false, m_viewportHovered = false;
		glm::vec2 m_viewportSize = { 0.0f, 0.0f };

		glm::vec4 m_squareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

		int m_gizmoType = -1;

		SceneHierarchyPanel m_sceneHierarchyPanel;
	};

}