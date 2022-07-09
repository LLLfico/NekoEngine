#include "SandBox2D.h"
#include "imgui/imgui.h"

SandBox2D::SandBox2D() : Layer("SandBox2D") {
}

void SandBox2D::OnAttach() {
	m_cameraController.SetOrthographicCamera();
}

void SandBox2D::OnDetach() {
}

void SandBox2D::OnUpdate(Neko::TimeStep dt) {
	m_cameraController.OnUpdate(dt);

	Neko::RenderCommand::SetClearColor({ 0.2f, 0.3f, 0.3f, 1.0f });
	Neko::RenderCommand::Clear();

	Neko::Renderer2D::BeginScene(m_cameraController.GetCamera());
	Neko::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, m_squareColor);
	Neko::Renderer2D::EndScene();

}

void SandBox2D::OnImGuiRender() {
	ImGui::Begin("Color Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_squareColor));
	ImGui::End();
}

void SandBox2D::OnEvent(Neko::Event& e) {
	m_cameraController.OnEvent(e);
}