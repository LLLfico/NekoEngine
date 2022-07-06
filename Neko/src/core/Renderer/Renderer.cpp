#include "pch.h"
#include "Renderer.h"

#include "RenderCommand.h"
#include "VertexArray.h"
#include "Camera.h"
#include "Shader.h"


namespace Neko {

	Renderer::SceneData* Renderer::s_sceneData = new Renderer::SceneData;

	void Renderer::Init() {
		RenderCommand::Init();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
		RenderCommand::SetViewPort(width, height);
	}

	void Renderer::BeginScene(const OrthographicCamera& camera) {
		s_sceneData->cameraMatrix = camera.GetMatrix();
	}

	void Renderer::EndScene() {
	}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform) {
		shader->Bind();

		shader->SetMat4("u_viewProjection", s_sceneData->cameraMatrix);
		shader->SetMat4("u_transform", transform);

		vertexArray->Bind();
		RenderCommand::DrawElement(vertexArray);
	}

}