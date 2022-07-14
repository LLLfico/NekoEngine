#include "pch.h"
#include "EditorLayer.h"

#include <imgui/imgui.h>

namespace Neko {

	EditorLayer::EditorLayer() : Layer("EditorLayer") {
	}

	void EditorLayer::OnAttach() {
		m_cameraController.SetOrthographicCamera();

		m_texture = Texture2D::Create("assets/textures/testpic.png");

		Neko::FrameBufferDesc desc = { 1280, 720 };
		m_framebuffer = Neko::FrameBuffer::Create(desc);

		m_scene = std::make_shared<Scene>();

		auto square = m_scene->CreateEntity("Green Square");
		square.AddComponent<SpriteRendererComponent>(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

		auto redSquare = m_scene->CreateEntity("Red Square");
		redSquare.AddComponent<SpriteRendererComponent>(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

		m_squareEntity = square;

		m_cametaEntity = m_scene->CreateEntity("Camera A");
		m_cametaEntity.AddComponent<CameraComponent>();

		m_secondCamera = m_scene->CreateEntity("Camera B");
		auto& cc = m_secondCamera.AddComponent<CameraComponent>();
		cc.primary = false;

		class CameraCtrler : public ScriptableEntity {
		public:
			virtual void OnCreate() override {
				auto& transform = GetComponent<TransformComponent>().transform;
				transform[3][0] = rand() % 10 - 5.0f;
			}
			virtual void OnDestroy() override {}
			virtual void OnUpdate(TimeStep dt) override {
				auto& transform = GetComponent<TransformComponent>().transform;
				float speed = 0.5f;

				if (Input::IsKeyPressed(NEKO_KEY_A))
					transform[3][0] -= speed * dt;
				if (Input::IsKeyPressed(NEKO_KEY_D))
					transform[3][0] += speed * dt;
				if (Input::IsKeyPressed(NEKO_KEY_W))
					transform[3][1] += speed * dt;
				if (Input::IsKeyPressed(NEKO_KEY_S))
					transform[3][1] -= speed * dt;
			}
		};

		m_cametaEntity.AddComponent<NativeScriptComponent>().Bind<CameraCtrler>();
		m_secondCamera.AddComponent<NativeScriptComponent>().Bind<CameraCtrler>();

		m_sceneHierarchyPanel.SetContext(m_scene);
	}

	void EditorLayer::OnDetach() {
	}

	void EditorLayer::OnUpdate(TimeStep dt) {

		if (FrameBufferDesc desc = m_framebuffer->GetDesc();
			m_viewportSize.x > 0.0f && m_viewportSize.y > 0.0f &&
			(desc.width != m_viewportSize.x || desc.height != m_viewportSize.y)) {
			m_framebuffer->Resize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);
			m_cameraController.OnResize(m_viewportSize.x, m_viewportSize.y);
			m_scene->OnViewportResize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);
		}

		if (m_viewportFocused)
			m_cameraController.OnUpdate(dt);

		Neko::Renderer2D::ResetStatistics();
		m_framebuffer->Bind();
		Neko::RenderCommand::SetClearColor({ 0.2f, 0.3f, 0.3f, 1.0f });
		Neko::RenderCommand::Clear();

		// Neko::Renderer2D::BeginScene(m_cameraController.GetCamera());
		/*Neko::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Neko::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		Neko::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_texture, 10.0f);*/

		m_scene->OnUpdate(dt);

		// Neko::Renderer2D::EndScene();

		Neko::Renderer2D::BeginScene(m_cameraController.GetCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f) {
			for (float x = -5.0f; x < 5.0f; x += 0.5f) {
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				Neko::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);\
			}
		}
		Neko::Renderer2D::EndScene();
		m_framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender() {
		// Note: Switch this to true to enable dockspace
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("Exit")) Neko::Application::GetCurrent().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		m_sceneHierarchyPanel.OnImGuiRender();

		ImGui::Begin("Status");

		auto stats = Neko::Renderer2D::GetStatistics();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.drawCalls);
		ImGui::Text("Quads: %d", stats.quadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");

		m_viewportFocused = ImGui::IsWindowFocused();
		m_viewportHovered = ImGui::IsWindowHovered();
		Application::GetCurrent().GetImGuiLayer()->BlockEvent(!m_viewportFocused || !m_viewportHovered);
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

		m_viewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint32_t textureID = m_framebuffer->GetColorAttachmentId();
		ImGui::Image((void*)textureID, ImVec2{ m_viewportSize.x, m_viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e) {
		m_cameraController.OnEvent(e);
	}

}