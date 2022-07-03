#include <Neko.h>

#include "imgui/imgui.h"

class ExampleLayer : public Neko::Layer {
public:
	ExampleLayer() : Layer("Example"), m_camera(Neko::Camera::Create(Neko::CameraInfo({ -1.6f, 1.6f, -0.9f, 0.9f }))), m_cameraPosition({ 0.0f, 0.0f, 0.0f }) {

		m_vao = Neko::VertexArray::Create();

		float vertices[] = {
			-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 0.5f, 0.0f,	   0.0f, 0.0f, 1.0f, 1.0f,
		};

		unsigned int indices[] = {
			0, 1, 2,
		};
		std::shared_ptr<Neko::VertexBuffer> vertexBuffer = Neko::VertexBuffer::Create(vertices, sizeof(vertices));

		Neko::BufferLayout layout = {
				{"a_position", Neko::ShaderDataType::Float3},
				{"a_color", Neko::ShaderDataType::Float4},
		};
		vertexBuffer->SetLayout(layout);
		m_vao->AddVertexBuffer(vertexBuffer);

		std::shared_ptr<Neko::IndexBuffer> indexBuffer = Neko::IndexBuffer::Create(indices, sizeof(indices));
		m_vao->SetIndexBuffer(indexBuffer);

		float squareVertices[] = {
			-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f,		1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f,		1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f,		0.0f, 1.0f,
		};
		m_squareVAO = Neko::VertexArray::Create();
		m_squareVAO->Bind();
		std::shared_ptr<Neko::VertexBuffer> squareVB = Neko::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		squareVB->SetLayout({
			{ "a_Position", Neko::ShaderDataType::Float3, },
			{ "a_TexCoord", Neko::ShaderDataType::Float2, },
			});
		m_squareVAO->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Neko::IndexBuffer> squareIB = Neko::IndexBuffer::Create(squareIndices, sizeof(squareIndices));
		m_squareVAO->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 460 core
			
			layout(location = 0) in vec3 a_position;
			layout(location = 1) in vec4 a_color;
			uniform mat4 u_viewProjection;
			uniform mat4 u_transform;
			out vec3 v_position;
			out vec4 v_color;
			void main()
			{
				v_position = a_position;
				v_color = a_color;
				gl_Position = u_viewProjection * u_transform * vec4(a_position, 1.0);	
			}
		)";

		std::string fragmentSrc = R"(
			#version 460 core
			
			layout(location = 0) out vec4 color;
			in vec3 v_position;
			in vec4 v_color;
			void main()
			{
				color = vec4(v_position * 0.5 + 0.5, 1.0);
				color = v_color;
			}
		)";

		m_shader = Neko::Shader::Create("vertexArrayTest", vertexSrc, fragmentSrc);

		auto textureShader = m_shaderManager.Load("assets/shaders/texture.glsl");
		// m_blueShader = Neko::Shader::Create("assets/shaders/texture.glsl");
		m_texture = Neko::Texture2D::Create("assets/textures/testpic.png");
		textureShader->SetInt("u_Texture", 0);

		m_transparentTexture = Neko::Texture2D::Create("assets/textures/blending_transparent_window.png");
	}

	void OnUpdate(Neko::Time t) override {
		// NEKO_INFO("delta time is {}", t.GetSeconds());

		if (Neko::Input::IsKeyPressed(NEKO_KEY_LEFT))
			m_cameraPosition.x -= m_cameraMoveSpeed * t;
		else if (Neko::Input::IsKeyPressed(NEKO_KEY_RIGHT))
			m_cameraPosition.x += m_cameraMoveSpeed * t;

		if (Neko::Input::IsKeyPressed(NEKO_KEY_UP))
			m_cameraPosition.y += m_cameraMoveSpeed * t;
		else if (Neko::Input::IsKeyPressed(NEKO_KEY_DOWN))
			m_cameraPosition.y -= m_cameraMoveSpeed * t;

		if (Neko::Input::IsKeyPressed(NEKO_KEY_A))
			m_cameraRotation.z += m_cameraRotateSpeed * t;
		else if (Neko::Input::IsKeyPressed(NEKO_KEY_D))
			m_cameraRotation.z -= m_cameraRotateSpeed * t;

		Neko::RenderCommand::SetClearColor(glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));
		Neko::RenderCommand::Clear();

		m_camera->SetPosition(m_cameraPosition);
		m_camera->SetRotation(m_cameraRotation);

		Neko::Renderer::BeginScene(m_camera);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		auto textureShader = m_shaderManager.GetShader("texture");
		textureShader->Bind();
		// m_blueShader->SetVec3("u_color", m_squareColor);
		m_texture->Bind();
		for (int i = -10; i < 10; i++) {
			for (int j = -10; j < 10; j++) {
				glm::vec3 pos(i * 0.11f, j * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Neko::Renderer::Submit(textureShader, m_squareVAO, transform);
			}
		}

		Neko::Renderer::Submit(textureShader, m_squareVAO, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		m_transparentTexture->Bind();
		Neko::Renderer::Submit(textureShader, m_squareVAO, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		Neko::Renderer::EndScene();
	}

	void OnImGuiRender() override {
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_squareColor));
		ImGui::End();
	}

	void OnEvent(Neko::Event& event) override {
		// NEKO_TRACE("{0}", event);

	}

private:
	Neko::ShaderManager m_shaderManager;

	std::shared_ptr<Neko::Shader> m_shader;
	std::shared_ptr<Neko::VertexArray> m_vao;
	std::shared_ptr<Neko::VertexBuffer> m_vertexBuffer;

	std::shared_ptr<Neko::VertexArray> m_squareVAO;

	std::shared_ptr<Neko::Camera> m_camera;
	
	std::shared_ptr<Neko::Texture2D> m_texture;
	std::shared_ptr<Neko::Texture2D> m_transparentTexture;

	glm::vec3 m_cameraPosition = glm::vec3(0.0f);
	glm::vec3 m_cameraRotation = glm::vec3(0.0f);
	float m_cameraMoveSpeed = 5.0f;
	float m_cameraRotateSpeed = 180.0f;

	glm::vec3 m_squareColor = { 0.2f, 0.3f, 0.8f };

};

class SandBox : public Neko::Application {
public:
	SandBox() {
		PushLayer(new ExampleLayer());
	}
	~SandBox() {}
};


Neko::Application* Neko::CreateApplication() {
	return new SandBox();
}