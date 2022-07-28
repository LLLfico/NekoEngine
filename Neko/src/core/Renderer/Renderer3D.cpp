#include "pch.h"
#include "Renderer3D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "world/Mesh.h"
#include "world/Scene.h"
#include <glad/glad.h>

namespace Neko {

	std::shared_ptr<FrameBuffer> Renderer3D::s_shadowFbo = nullptr;

	struct Renderer3DData{
		std::shared_ptr<Shader> shaderModel;
		std::shared_ptr<Shader> shaderDepth;
	};

	static Renderer3DData s_data3d;

	void Renderer3D::Init() {
		NEKO_CORE_INFO("Renderer3D Init");
		s_data3d.shaderModel = Shader::Create("assets/shaders/ShaderModel.glsl");
		s_data3d.shaderDepth = Shader::Create("assets/shaders/ShaderDepth.glsl");

		FrameBufferDesc desc = { 1024, 1024, { FrameBufferTextureFormat::RGBA8 , FrameBufferTextureFormat::DEPTH } };
		s_shadowFbo = FrameBuffer::Create(desc);
	}

	void Neko::Renderer3D::BeginScene(const EditorCamera& camera) {
		glm::mat4 viewProjection = camera.GetMatrix();

		s_data3d.shaderModel->Bind();
		s_data3d.shaderModel->SetMat4("u_viewProjection", viewProjection);
		s_data3d.shaderModel->SetVec3("u_cameraPos", camera.GetPosition());

		// temp
		s_data3d.shaderModel->SetFloat("u_metallic", 0.90f);
		s_data3d.shaderModel->SetFloat("u_roughness", 0.4f);
		s_data3d.shaderModel->SetFloat("u_ao", 1.0f);

		BindDepthTexture(8);
	}

	void Renderer3D::BindDepthTexture(uint32_t slot) {
		s_data3d.shaderModel->Bind();
		s_data3d.shaderModel->SetInt("u_shadowMap", slot);
		s_shadowFbo->BindDepthTexture(8);
	}

	void Renderer3D::SetDirectionalLight(const glm::vec3& direction, const glm::vec3& radiance) {
		s_data3d.shaderModel->Bind();
		s_data3d.shaderModel->SetVec3("u_directionallight.direction", direction);
		s_data3d.shaderModel->SetVec3("u_directionallight.radiance", radiance);
	}

	void Renderer3D::SetLightSpaceMatrix(const glm::mat4& matrix) {
		s_data3d.shaderModel->Bind();
		s_data3d.shaderModel->SetMat4("u_lightSpaceMatrix", matrix);
	}

	void Renderer3D::SetPointLight(const glm::vec3& position, const glm::vec3& radiance, int index) {
		s_data3d.shaderModel->Bind();
		s_data3d.shaderModel->SetVec3("u_pointlights[" + std::to_string(index) + "].position", position);
		s_data3d.shaderModel->SetVec3("u_pointlights[" + std::to_string(index) + "].radiance", radiance);
	}

	void Renderer3D::SetPointLightNum(int num) {
		s_data3d.shaderModel->Bind();
		s_data3d.shaderModel->SetInt("u_pointLightsNum", num);
	}

	void Renderer3D::DrawMesh(const glm::mat4& transform, std::shared_ptr<Mesh> mesh, int entityId) {
		s_data3d.shaderModel->Bind();
		s_data3d.shaderModel->SetMat4("u_model", transform);
		mesh->Draw(transform, s_data3d.shaderModel, entityId);
	}

	void Neko::Renderer3D::EndScene() {
	}

}
