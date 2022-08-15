#include <Neko.h>
#include <core/Launcher.h>

#include "imgui/imgui.h"

#include "SandBox2D.h"

class ExampleLayer : public Neko::Layer {
public:
	ExampleLayer() : Layer("Example") {

		float cubeVertices[] = {
			// ----position----    -------normal------    ----uv----
			-1.0f, -1.0f, -1.0f,   +0.0f, -1.0f, +0.0f,   0.0f, 0.0f,
			-1.0f, -1.0f, +1.0f,   +0.0f, -1.0f, +0.0f,   0.0f, 1.0f,
			+1.0f, -1.0f, +1.0f,   +0.0f, -1.0f, +0.0f,   1.0f, 1.0f,
			+1.0f, -1.0f, -1.0f,   +0.0f, -1.0f, +0.0f,   1.0f, 0.0f,
			-1.0f, +1.0f, -1.0f,   +0.0f, +1.0f, +0.0f,   1.0f, 0.0f,
			-1.0f, +1.0f, +1.0f,   +0.0f, +1.0f, +0.0f,   1.0f, 1.0f,
			+1.0f, +1.0f, +1.0f,   +0.0f, +1.0f, +0.0f,   0.0f, 1.0f,
			+1.0f, +1.0f, -1.0f,   +0.0f, +1.0f, +0.0f,   0.0f, 0.0f,
			-1.0f, -1.0f, -1.0f,   +0.0f, +0.0f, -1.0f,   0.0f, 0.0f,
			-1.0f, +1.0f, -1.0f,   +0.0f, +0.0f, -1.0f,   0.0f, 1.0f,
			+1.0f, +1.0f, -1.0f,   +0.0f, +0.0f, -1.0f,   1.0f, 1.0f,
			+1.0f, -1.0f, -1.0f,   +0.0f, +0.0f, -1.0f,   1.0f, 0.0f,
			-1.0f, -1.0f, +1.0f,   +0.0f, +0.0f, +1.0f,   0.0f, 0.0f,
			-1.0f, +1.0f, +1.0f,   +0.0f, +0.0f, +1.0f,   0.0f, 1.0f,
			+1.0f, +1.0f, +1.0f,   +0.0f, +0.0f, +1.0f,   1.0f, 1.0f,
			+1.0f, -1.0f, +1.0f,   +0.0f, +0.0f, +1.0f,   1.0f, 0.0f,
			-1.0f, -1.0f, -1.0f,   -1.0f, +0.0f, +0.0f,   0.0f, 0.0f,
			-1.0f, -1.0f, +1.0f,   -1.0f, +0.0f, +0.0f,   0.0f, 1.0f,
			-1.0f, +1.0f, +1.0f,   -1.0f, +0.0f, +0.0f,   1.0f, 1.0f,
			-1.0f, +1.0f, -1.0f,   -1.0f, +0.0f, +0.0f,   1.0f, 0.0f,
			+1.0f, -1.0f, -1.0f,   +1.0f, +0.0f, +0.0f,   0.0f, 0.0f,
			+1.0f, -1.0f, +1.0f,   +1.0f, +0.0f, +0.0f,   0.0f, 1.0f,
			+1.0f, +1.0f, +1.0f,   +1.0f, +0.0f, +0.0f,   1.0f, 1.0f,
			+1.0f, +1.0f, -1.0f,   +1.0f, +0.0f, +0.0f,   1.0f, 0.0f
		};
		m_cubeVAO = Neko::VertexArray::Create();
		m_cubeVAO->Bind();
		std::shared_ptr<Neko::VertexBuffer> squareVB = Neko::VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));
		squareVB->SetLayout({
			{ "a_position", Neko::ShaderDataType::Float3, },
			{ "a_normal", Neko::ShaderDataType::Float3, },
			{ "a_texcoord", Neko::ShaderDataType::Float2, },
			});
		m_cubeVAO->AddVertexBuffer(squareVB);

		uint32_t squareIndices[] = {
			+0, +2, +1,   +0, +3, +2,   +4, +5, +6,
			+4, +6, +7,   +8, +9, 10,   +8, 10, 11,
			12, 15, 14,   12, 14, 13,   16, 17, 18,
			16, 18, 19,   20, 23, 22,   20, 22, 21
		};
		std::shared_ptr<Neko::IndexBuffer> cubeIBO = Neko::IndexBuffer::Create(squareIndices, sizeof(squareIndices));
		m_cubeVAO->SetIndexBuffer(cubeIBO);

		auto textureShader = m_shaderManager.Load("assets/shaders/texture.glsl");
		m_texture = Neko::Texture2D::Create("assets/textures/testpic.png");
		textureShader->SetInt("u_texture", 0);

		m_transparentTexture = Neko::Texture2D::Create("assets/textures/blending_transparent_window.png");
	}

	void OnUpdate(Neko::TimeStep dt) override {
		// NEKO_INFO("delta time is {}", t.GetSeconds());
		m_cameraController.OnUpdate(dt);
		

		Neko::RenderCommand::SetClearColor(glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));
		Neko::RenderCommand::Clear();

		Neko::Renderer::BeginScene(m_cameraController.GetCamera());

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		auto textureShader = m_shaderManager.GetShader("texture");
		textureShader->Bind();
		// m_blueShader->SetVec3("u_color", m_squareColor);
		m_texture->Bind();
		/*for (int i = -10; i < 10; i++) {
			for (int j = -10; j < 10; j++) {
				glm::vec3 pos(i * 0.11f, j * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Neko::Renderer::Submit(textureShader, m_squareVAO, transform);
			}
		}*/

		Neko::Renderer::Submit(textureShader, m_cubeVAO);

		// m_transparentTexture->Bind();
		// Neko::Renderer::Submit(textureShader, m_cubeVAO);

		Neko::Renderer::EndScene();
	}

	void OnImGuiRender() override {
		ImGui::Begin("Camera Position");
		auto position = m_cameraController.GetCamera().GetPosition();
		ImGui::Text("%f, %f, %f", position[0], position[1], position[2]);
		ImGui::End();
	}

	void OnEvent(Neko::Event& event) override {
		// NEKO_TRACE("{0}", event);
		m_cameraController.OnEvent(event);
	}

private:
	Neko::ShaderLibrary m_shaderManager;

	std::shared_ptr<Neko::VertexArray> m_cubeVAO;

	Neko::CameraController m_cameraController;
	
	std::shared_ptr<Neko::Texture2D> m_texture;
	std::shared_ptr<Neko::Texture2D> m_transparentTexture;

	glm::vec3 m_squareColor = { 0.2f, 0.3f, 0.8f };

};

class SandBox : public Neko::Application {
public:
	SandBox() {
		// PushLayer(new ExampleLayer());
		PushLayer(new SandBox2D());
	}
	~SandBox() {}
};


Neko::Application* Neko::CreateApplication() {
	return new SandBox();
}