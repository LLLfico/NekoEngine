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
#include <glm/gtc/matrix_transform.hpp>

namespace Neko {

	std::shared_ptr<FrameBuffer> Renderer3D::s_shadowFbo = nullptr;

	struct Renderer3DData{
		std::shared_ptr<Shader> shaderModel;
		std::shared_ptr<Shader> shaderDepth;
		std::shared_ptr<Shader> shaderCubeMap;
		std::shared_ptr<Shader> shaderHdr;

		std::shared_ptr<TextureCubeMap> textureCubeMap;
		std::shared_ptr<Texture2D> textureHdr;

		std::shared_ptr<Mesh> backgroundCube;
		std::shared_ptr<Mesh> showHdrCube;

		uint32_t captureFBO, captureRBO;
		uint32_t envCubemap;
	};

	static Renderer3DData s_data3d;

	void Renderer3D::Init() {
		NEKO_CORE_INFO("Renderer3D Init");
		s_data3d.shaderModel = Shader::Create("assets/shaders/ShaderModel.glsl");
		s_data3d.shaderDepth = Shader::Create("assets/shaders/ShaderDepth.glsl");
		s_data3d.shaderCubeMap = Shader::Create("assets/shaders/ShaderBackground.glsl");
		s_data3d.shaderHdr = Shader::Create("assets/shaders/ShaderHDR.glsl");

		s_data3d.backgroundCube = std::make_shared<Mesh>("assets/models/basic_geometry/cube.obj");
		s_data3d.showHdrCube = std::make_shared<Mesh>("assets/models/basic_geometry/cube.obj");

		std::vector<std::string> cubeMapPaths = {
			"assets/textures/cubemap/beach/px.png", // left
			"assets/textures/cubemap/beach/nx.png", // right 
			"assets/textures/cubemap/beach/py.png", // top
			"assets/textures/cubemap/beach/ny.png", // bottom
			"assets/textures/cubemap/beach/pz.png", // back
			"assets/textures/cubemap/beach/nz.png", // front
		};

		s_data3d.textureCubeMap = TextureCubeMap::Create(cubeMapPaths);
		s_data3d.textureHdr = Texture2D::Create("assets/textures/hdr/newport_loft/Newport_Loft_Ref.hdr");

		FrameBufferDesc descShadow = { 1024, 1024, { FrameBufferTextureFormat::RGBA8 , FrameBufferTextureFormat::DEPTH } };
		s_shadowFbo = FrameBuffer::Create(descShadow);

		uint32_t captureWidth = 512, captureHeight = 512;
		glGenFramebuffers(1, &s_data3d.captureFBO);
		glGenRenderbuffers(1, &s_data3d.captureRBO);
		glBindFramebuffer(GL_FRAMEBUFFER, s_data3d.captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, s_data3d.captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, captureWidth, captureHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, s_data3d.captureRBO);

		glGenTextures(1, &s_data3d.envCubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, s_data3d.envCubemap);
		for (unsigned int i = 0; i < 6; ++i) {
			// note that we store each face with 16 bit floating point values
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, captureWidth, captureHeight, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] =
		{
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		s_data3d.shaderHdr->Bind();
		s_data3d.shaderHdr->SetMat4("u_projection", captureProjection);
		s_data3d.shaderHdr->SetInt("u_equirectangularMap", 0);
		s_data3d.textureHdr->Bind(0);

		glViewport(0, 0, captureWidth, captureHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, s_data3d.captureFBO);
		for (size_t i = 0; i < 6; i++) {
			s_data3d.shaderHdr->SetMat4("u_view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, s_data3d.envCubemap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			s_data3d.showHdrCube->Draw(glm::mat4(1.0f), s_data3d.shaderHdr, -1);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Neko::Renderer3D::BeginScene(const EditorCamera& camera) {
		glm::mat4 viewProjection = camera.GetMatrix();

		s_data3d.shaderModel->Bind();
		s_data3d.shaderModel->SetMat4("u_viewProjection", viewProjection);
		s_data3d.shaderModel->SetVec3("u_cameraPos", camera.GetPosition());

		// temp
		s_data3d.shaderModel->SetFloat("u_metallic", 0.90f);
		s_data3d.shaderModel->SetFloat("u_roughness", 0.04f);
		s_data3d.shaderModel->SetFloat("u_ao", 1.0f);
		BindDepthTexture(8);

		glm::mat4 backgroundVPMatrix = camera.GetProjection() * glm::mat4(glm::mat3(camera.GetViewMatrix()));
		s_data3d.shaderCubeMap->Bind();
		s_data3d.shaderCubeMap->SetMat4("u_viewProjection", backgroundVPMatrix);
		s_data3d.shaderCubeMap->SetInt("u_cubemap", 0);
		glBindTextureUnit(0, s_data3d.envCubemap);
		// s_data3d.textureCubeMap->Bind(0);

		/*s_data3d.shaderHdr->Bind();
		s_data3d.shaderHdr->SetMat4("u_view", camera.GetViewMatrix());
		s_data3d.shaderHdr->SetMat4("u_projection", camera.GetProjection());
		s_data3d.shaderHdr->SetInt("u_equirectangularMap", 1);
		s_data3d.textureHdr->Bind(1);

		s_data3d.showHdrCube->Draw(glm::mat4(1.0f), s_data3d.shaderHdr, -1);*/
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
		glDepthFunc(GL_LEQUAL);
		s_data3d.backgroundCube->Draw(glm::mat4(1.0f), s_data3d.shaderCubeMap, -1);
		glDepthFunc(GL_LESS);
	}

}
