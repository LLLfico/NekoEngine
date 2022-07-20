#include "pch.h"
#include "EditorLayer.h"

#include "core/utils/PlatformUtils.h"
#include "core/math/Math.h"
#include "world/SceneSerializer.h"

#include <imgui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>

namespace Neko {

	extern const std::filesystem::path g_assetPath;

	EditorLayer::EditorLayer() : Layer("EditorLayer") {
	}

	void EditorLayer::OnAttach() {
		m_cameraController.SetOrthographicCamera();

		m_texture = Texture2D::Create("assets/textures/testpic.png");
		m_playIcon = Texture2D::Create("resources/icon/PlayButton.png");
		m_stopIcon = Texture2D::Create("resources/icon/StopButton.png");

		Neko::FrameBufferDesc desc = { 1280, 720, {FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::DEPTH24STENCIL8} };
		m_framebuffer = Neko::FrameBuffer::Create(desc);

		m_scene = std::make_shared<Scene>();
		m_editorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
		#if 0
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
				auto& translation = GetComponent<TransformComponent>().translation;
				translation.x = rand() % 10 - 5.0f;
			}
			virtual void OnDestroy() override {}
			virtual void OnUpdate(TimeStep dt) override {
				auto& translation = GetComponent<TransformComponent>().translation;
				float speed = 0.5f;

				if (Input::IsKeyPressed(Key::A))
					translation.x -= speed * dt;
				if (Input::IsKeyPressed(Key::D))
					translation.x += speed * dt;
				if (Input::IsKeyPressed(Key::W))
					translation.y += speed * dt;
				if (Input::IsKeyPressed(Key::S))
					translation.y -= speed * dt;
			}
		};

		m_cametaEntity.AddComponent<NativeScriptComponent>().Bind<CameraCtrler>();
		m_secondCamera.AddComponent<NativeScriptComponent>().Bind<CameraCtrler>();
		#endif
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
			m_editorCamera.SetViewportSize(m_viewportSize.x, m_viewportSize.y);
			m_scene->OnViewportResize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);
		}

		if (m_viewportFocused)
			m_cameraController.OnUpdate(dt);

		m_editorCamera.OnUpdate(dt);

		Neko::Renderer2D::ResetStatistics();
		m_framebuffer->Bind();
		Neko::RenderCommand::SetClearColor({ 0.2f, 0.3f, 0.3f, 1.0f });
		Neko::RenderCommand::Clear();

		m_framebuffer->ClearAttachment(1, -1);

		switch (m_sceneState) {
			case SceneState::Edit: {
				if (m_viewportFocused) {
					m_cameraController.OnUpdate(dt);
				}
				m_editorCamera.OnUpdate(dt);

				m_scene->OnUpdateEditor(dt, m_editorCamera);
				break;
			}
			case SceneState::Play: {
				m_scene->OnUpdateRuntime(dt);
				break;
			}
		}

		// Neko::Renderer2D::BeginScene(m_cameraController.GetCamera());
		/*Neko::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Neko::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		Neko::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_texture, 10.0f);*/

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_viewportBounds[0].x;
		my -= m_viewportBounds[0].y;
		glm::vec2 viewportSize = m_viewportBounds[1] - m_viewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseX < (int)viewportSize.x && mouseY >= 0 && mouseY < (int)viewportSize.y) {
			int pixelData = m_framebuffer->ReadPixel(1, mouseX, mouseY);
			// NEKO_CORE_WARN("Pixel data = {0}", pixelData);
			m_hoveredEntity = (pixelData == -1) ? Entity() : Entity((entt::entity)pixelData, m_scene.get());
		}

		// Neko::Renderer2D::EndScene();

		/*Neko::Renderer2D::BeginScene(m_cameraController.GetCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f) {
			for (float x = -5.0f; x < 5.0f; x += 0.5f) {
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				Neko::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);\
			}
		}
		Neko::Renderer2D::EndScene();*/
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
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
				if (ImGui::MenuItem("New", "Ctrl+N")) {
					NewScene();
				}
				if (ImGui::MenuItem("Open", "Ctrl+O")) {
					OpenScene();
				}
				if (ImGui::MenuItem("Save As...", "Ctrl+SHift+S")) {
					SaveScene();
				}

				if (ImGui::MenuItem("Exit")) Neko::Application::GetCurrent().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		m_sceneHierarchyPanel.OnImGuiRender();
		m_contentBrowserPanel.OnImGuiRender();

		ImGui::Begin("Status");

		std::string name = "None";
		if (m_hoveredEntity)
			name = m_hoveredEntity.GetComponent<TagComponent>().tag;
		ImGui::Text("Hovered Entity : %s", name.c_str());

		auto stats = Neko::Renderer2D::GetStatistics();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.drawCalls);
		ImGui::Text("Quads: %d", stats.quadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_viewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_viewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_viewportFocused = ImGui::IsWindowFocused();
		m_viewportHovered = ImGui::IsWindowHovered();
		Application::GetCurrent().GetImGuiLayer()->BlockEvent(!m_viewportFocused && !m_viewportHovered);
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

		m_viewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint32_t textureID = m_framebuffer->GetColorAttachmentId(0);
		ImGui::Image((void*)textureID, ImVec2{ m_viewportSize.x, m_viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
				const wchar_t* path = (const wchar_t*)payload->Data;
				OpenScene(g_assetPath / path);
			}
			ImGui::EndDragDropTarget();
		}

		// gizmos
		Entity selectEntity = m_sceneHierarchyPanel.GetSelectedEntity();
		if (selectEntity && m_gizmoType != -1) {
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(m_viewportBounds[0].x, m_viewportBounds[0].y, m_viewportBounds[1].x - m_viewportBounds[0].x, m_viewportBounds[1].y - m_viewportBounds[0].y);
			// ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			// camera
			const glm::mat4& cameraProjection = m_editorCamera.GetProjection();
			glm::mat4 cameraView = m_editorCamera.GetViewMatrix();
			

			// entity transform
			auto& tc = selectEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransformMatrix();

			// snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f;
			if (m_gizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;
			float snapValues[3] = { snapValue , snapValue , snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), (ImGuizmo::OPERATION)m_gizmoType,
								ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr);
			if (ImGuizmo::IsUsing()) {
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.rotation;
				tc.translation = translation;
				tc.rotation += deltaRotation;
				tc.scale = scale;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();

		UI_Toolbar();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e) {
		// m_cameraController.OnEvent(e);
		m_editorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(NEKO_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(NEKO_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e) {
		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		switch (e.GetKeyCode()) {
			case Key::N: {
				if (control)
					NewScene();
				break;
			}
			case Key::O: {
				if (control)
					OpenScene();
				break;
			}
			case Key::S: {
				if (control && shift)
					SaveScene();
				break;
			}
			case Key::Q: {
				if (!ImGuizmo::IsUsing())
					m_gizmoType = -1;
				break;
			}
			case Key::W: {
				if (!ImGuizmo::IsUsing())
					m_gizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			}
			case Key::E: {
				if (!ImGuizmo::IsUsing())
					m_gizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			}
			case Key::R: {
				if (!ImGuizmo::IsUsing())
					m_gizmoType = ImGuizmo::OPERATION::SCALE;
				break;
			}
		}
		return true;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonEvent& e) {
		if (e.GetMouseButton() == Mouse::ButtonLeft) {
			if (m_viewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt)) {
				m_sceneHierarchyPanel.SetSelectedEntity(m_hoveredEntity);
			}
		}
		return false;
	}

	void EditorLayer::NewScene() {
		m_scene = std::make_shared<Scene>();
		m_scene->OnViewportResize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);
		m_sceneHierarchyPanel.SetContext(m_scene);
	}

	void EditorLayer::OpenScene() {
		auto filepath = FileDialogs::OpenFile("Neko Scene (*.neko)\0*.neko\0");
		if (filepath)
			OpenScene(*filepath);
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path) {
		if (path.extension().string() != ".neko") {
			NEKO_WARN("Not a neko scene file {0}", path.filename().string());
			return;
		}
		auto newScene = std::make_shared<Scene>();
		newScene->OnViewportResize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string())) {
			m_scene = newScene;
			m_sceneHierarchyPanel.SetContext(m_scene);
		}
	}

	void EditorLayer::SaveScene() {
		auto filepath = FileDialogs::SaveFile("Neko Scene (*.neko)\0*.neko\0");
		if (filepath)
			return;
		SceneSerializer serializer(m_scene);
		serializer.Serialize(*filepath);
	}

	void EditorLayer::OnScenePlay() {
		m_sceneState = SceneState::Play;
		m_scene->OnRuntimeStart();
	}

	void EditorLayer::OnSceneStop() {
		m_sceneState = SceneState::Edit;
		m_scene->OnRuntimeStop();
	}

	void EditorLayer::UI_Toolbar() {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		float size = ImGui::GetWindowHeight() - 4.0f;
		std::shared_ptr<Texture2D> icon = m_sceneState == SceneState::Edit ? m_playIcon : m_stopIcon;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
		if (ImGui::ImageButton((ImTextureID)icon->GetId(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0)) {
			if (m_sceneState == SceneState::Edit)
				OnScenePlay();
			else if (m_sceneState == SceneState::Play)
				OnSceneStop();
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

}