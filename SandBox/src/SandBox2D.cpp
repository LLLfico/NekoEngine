#include "SandBox2D.h"
#include "imgui/imgui.h"

SandBox2D::SandBox2D() : Layer("SandBox2D") {
}

void SandBox2D::OnAttach() {
	m_cameraController.SetOrthographicCamera();

	m_texture = Neko::Texture2D::Create("assets/textures/testpic.png");
}

void SandBox2D::OnDetach() {
}

void SandBox2D::OnUpdate(Neko::TimeStep dt) {
	m_cameraController.OnUpdate(dt);

	Neko::Renderer2D::ResetStatistics();

	Neko::RenderCommand::SetClearColor({ 0.2f, 0.3f, 0.3f, 1.0f });
	Neko::RenderCommand::Clear();

	Neko::Renderer2D::BeginScene(m_cameraController.GetCamera());
	Neko::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
	Neko::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
	Neko::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_texture, 10.0f);
	Neko::Renderer2D::EndScene();

	Neko::Renderer2D::BeginScene(m_cameraController.GetCamera());
	for (float y = -5.0f; y < 5.0f; y += 0.5f) {
		for (float x = -5.0f; x < 5.0f; x += 0.5f) {
			glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
			Neko::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
		}
	}
	Neko::Renderer2D::EndScene();
}

void SandBox2D::OnImGuiRender() {
	ImGui::Begin("Color Settings");

	auto stats = Neko::Renderer2D::GetStatistics();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.drawCalls);
	ImGui::Text("Quads: %d", stats.quadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_squareColor));

	ImGui::End();
}

void SandBox2D::OnEvent(Neko::Event& e) {
	m_cameraController.OnEvent(e);
}