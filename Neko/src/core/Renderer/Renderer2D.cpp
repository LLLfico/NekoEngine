#include "pch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

namespace Neko {

	struct Renderer2DStorage {
		std::shared_ptr<VertexArray> quadVertexArray;
		std::shared_ptr<Shader> flatColorShader;
	};

	static Renderer2DStorage* s_data;

	void Renderer2D::Init() {
		s_data = new Renderer2DStorage();
		s_data->quadVertexArray = VertexArray::Create();

		float vertices[] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f,
		};
		uint32_t indices[] = {
			 0, 1, 2,
			 2, 3, 0,
		};

		std::shared_ptr<VertexBuffer> vbo = VertexBuffer::Create(vertices, sizeof(vertices));
		vbo->SetLayout({ {"a_position", ShaderDataType::Float3} });
		s_data->quadVertexArray->AddVertexBuffer(vbo);
		std::shared_ptr<IndexBuffer> ibo = IndexBuffer::Create(indices, sizeof(indices));
		s_data->quadVertexArray->SetIndexBuffer(ibo);

		s_data->flatColorShader = Shader::Create("assets/shaders/FlatColor.glsl");
	}

	void Renderer2D::Shutdown() {
		delete s_data;
	}

	void Renderer2D::BeginScene(const Camera& camera) {
		s_data->flatColorShader->Bind();
		s_data->flatColorShader->SetMat4("u_viewProjection", camera.GetMatrix());
		s_data->flatColorShader->SetMat4("u_transform", glm::mat4(1.0f));
	}

	void Renderer2D::EndScene() {

	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) {
		s_data->flatColorShader->Bind();
		s_data->flatColorShader->SetVec4("u_color", color);

		s_data->quadVertexArray->Bind();
		RenderCommand::DrawElement(s_data->quadVertexArray);
	}

}