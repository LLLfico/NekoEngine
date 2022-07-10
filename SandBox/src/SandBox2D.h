#pragma once
#include "Neko.h"

class SandBox2D : public Neko::Layer {
public:
	SandBox2D();
	virtual ~SandBox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Neko::TimeStep dt) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Neko::Event& e) override;
private:
	Neko::CameraController m_cameraController;

	std::shared_ptr<Neko::VertexArray> m_squareVAO;
	std::shared_ptr<Neko::Shader> m_flatColorShader;

	std::shared_ptr<Neko::VertexArray> m_cubeVAO;
	std::shared_ptr<Neko::Shader> m_cubeShader;
	std::shared_ptr<Neko::Texture2D> m_texture;

	std::shared_ptr<Neko::FrameBuffer> m_framebuffer;

	glm::vec4 m_squareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
};