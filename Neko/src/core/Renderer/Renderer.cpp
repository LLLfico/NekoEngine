#include "pch.h"
#include "Renderer.h"

#include "RenderCommand.h"
#include "VertexArray.h"
#include "Camera.h"
#include "Shader.h"
#include "Renderer2D.h"


namespace Neko {

	std::unique_ptr<Renderer::SceneData> Renderer::s_sceneData = std::make_unique<Renderer::SceneData>();

	void Renderer::Init() {
		RenderCommand::Init();
		Renderer2D::Init();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
		RenderCommand::SetViewPort(width, height);
	}

	void Renderer::BeginScene(const Camera& camera) {
		s_sceneData->cameraMatrix = camera.GetMatrix();
	}

	void Renderer::EndScene() {
	}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform) {
		shader->Bind();

		shader->SetMat4("u_viewProjection", s_sceneData->cameraMatrix);
		shader->SetMat4("u_transform", transform);

		if (!vertexArray->GetIndexBuffer() || vertexArray->GetIndexBuffer()->Count() == 0) {
			NEKO_CORE_WARN("Renderer : Index buffer is empty!");
			return;
		}
		
		vertexArray->Bind();
		RenderCommand::DrawElement(vertexArray);
	}

}