#include "pch.h"
#include "Renderer3D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"
#include "Texture.h"
#include "world/Mesh.h"
#include <glad/glad.h>

namespace Neko {

	struct Renderer3DData{
		std::shared_ptr<Shader> shaderModel;
	};

	static Renderer3DData s_data;

	void Renderer3D::Init() {
		NEKO_CORE_INFO("Renderer3D Init");
		s_data.shaderModel = Shader::Create("assets/shaders/ShaderModel.glsl");
	}

	void Neko::Renderer3D::BeginScene(const EditorCamera& camera) {
		glm::mat4 viewProjection = camera.GetMatrix();

		s_data.shaderModel->Bind();
		s_data.shaderModel->SetMat4("u_viewProjection", viewProjection);
	}

	void Renderer3D::DrawMesh(const glm::mat4& transform, std::shared_ptr<Mesh> mesh, int entityId) {
		s_data.shaderModel->Bind();
		s_data.shaderModel->SetMat4("u_model", transform);
		mesh->Draw(transform, s_data.shaderModel, entityId);
	}

	void Neko::Renderer3D::EndScene() {
	}


}
