#include "pch.h"
#include "Renderer3D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "UniformBuffer.h"
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
		std::shared_ptr<Shader> shaderIradiance;
		std::shared_ptr<Shader> shaderPrefliter;
		std::shared_ptr<Shader> shaderBRDF;

		std::shared_ptr<TextureCubeMap> textureCubeMap;
		std::shared_ptr<Texture2D> textureHdr;

		std::shared_ptr<Mesh> backgroundCube;
		std::shared_ptr<Mesh> showHdrCube;

		uint32_t captureFBO, captureRBO;
		uint32_t envCubemap;
		uint32_t iradianceCubeMap;
		uint32_t prefliterMap;
		uint32_t brdfLUTTexture;

		std::shared_ptr<VertexArray> brdfVAO;
		std::shared_ptr<VertexBuffer> brdfVBO;
		std::shared_ptr<IndexBuffer> brdfIBO;
		std::shared_ptr<Shader> shaderQuad;
		struct CameraData {
			glm::mat4 viewProjection;
		};
		CameraData cameraBuffer;
		std::shared_ptr<UniformBuffer> cameraUniformBuffer;

		std::shared_ptr<VertexArray> talVAO;
		std::shared_ptr<VertexBuffer> talVBO;
		std::shared_ptr<IndexBuffer> talIBO;
	};

	static Renderer3DData s_data3d;

	void Renderer3D::Init() {
		NEKO_CORE_INFO("Renderer3D Init");
		s_data3d.shaderModel = Shader::Create("assets/shaders/ShaderModel.glsl");
		s_data3d.shaderDepth = Shader::Create("assets/shaders/ShaderDepth.glsl");
		s_data3d.shaderCubeMap = Shader::Create("assets/shaders/ShaderBackground.glsl");
		s_data3d.shaderHdr = Shader::Create("assets/shaders/ShaderHDR.glsl");
		s_data3d.shaderIradiance = Shader::Create("assets/shaders/ShaderIradiance.glsl");
		s_data3d.shaderPrefliter = Shader::Create("assets/shaders/ShaderPrefliter.glsl");
		s_data3d.shaderBRDF = Shader::Create("assets/shaders/ShaderBRDF.glsl");

		s_data3d.shaderQuad = Shader::Create("assets/shaders/ShaderQuad.glsl");

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

		// pre compute diffuse iradiance 
		const uint32_t iradianceSize = 512;
		glGenTextures(1, &s_data3d.iradianceCubeMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, s_data3d.iradianceCubeMap);
		for (unsigned int i = 0; i < 6; ++i) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, iradianceSize, iradianceSize, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, s_data3d.captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, s_data3d.captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, iradianceSize, iradianceSize);

		s_data3d.shaderIradiance->Bind();
		s_data3d.shaderIradiance->SetInt("u_cubemap", 0);
		s_data3d.shaderIradiance->SetMat4("u_projection", captureProjection);
		glBindTextureUnit(0, s_data3d.envCubemap);

		glViewport(0, 0, iradianceSize, iradianceSize);
		glBindFramebuffer(GL_FRAMEBUFFER, s_data3d.captureFBO);
		for (size_t i = 0; i < 6; i++) {
			s_data3d.shaderHdr->SetMat4("u_view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, s_data3d.iradianceCubeMap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			s_data3d.showHdrCube->Draw(glm::mat4(1.0f), s_data3d.shaderIradiance, -1);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// pre compute specular ibl : 
		// environment mipmap pass
		uint32_t prefliterSize = 128;
		glGenTextures(1, &s_data3d.prefliterMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, s_data3d.prefliterMap);
		for (size_t i = 0; i < 6; i++) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, prefliterSize, prefliterSize, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		s_data3d.shaderPrefliter->Bind();
		s_data3d.shaderPrefliter->SetInt("u_cubemap", 0);
		s_data3d.shaderPrefliter->SetMat4("u_projection", captureProjection);
		glBindTextureUnit(0, s_data3d.envCubemap);
		glBindFramebuffer(GL_FRAMEBUFFER, s_data3d.captureFBO);
		uint32_t maxMipmapLevels = 5;
		for (size_t mip = 0; mip < maxMipmapLevels; mip++) {
			uint32_t mipWidth = prefliterSize * std::pow(0.5, mip);
			uint32_t mipHeight = prefliterSize * std::pow(0.5, mip);
			glBindRenderbuffer(GL_RENDERBUFFER, s_data3d.captureRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipmapLevels - 1);
			s_data3d.shaderPrefliter->SetFloat("u_roughness", roughness);
			for (size_t i = 0; i < 6; i++) {
				s_data3d.shaderPrefliter->SetMat4("u_view", captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, s_data3d.prefliterMap, mip);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				s_data3d.showHdrCube->Draw(glm::mat4(1.0f), s_data3d.shaderPrefliter, -1);
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// brdf pass
		uint32_t brdfLUTSize = 512;
		glGenTextures(1, &s_data3d.brdfLUTTexture);
		glBindTexture(GL_TEXTURE_2D, s_data3d.brdfLUTTexture);
		// glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, brdfLUTSize, brdfLUTSize, 0, GL_RG, GL_FLOAT, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, brdfLUTSize, brdfLUTSize, 0, GL_RGB, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTextureUnit(10, s_data3d.brdfLUTTexture);

		glBindFramebuffer(GL_FRAMEBUFFER, s_data3d.captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, s_data3d.captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, brdfLUTSize, brdfLUTSize);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_data3d.brdfLUTTexture, 0);

		glViewport(0, 0, brdfLUTSize, brdfLUTSize);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// draw quad
		float brdfData[] = {
			-1.0f, -1.0f, +0.0f,   0.0f, 0.0f,
			+1.0f, -1.0f, +0.0f,   1.0f, 0.0f,
			+1.0f, +1.0f, +0.0f,   1.0f, 1.0f,
			-1.0f, +1.0f, +0.0f,   0.0f, 1.0f,
		};
		uint32_t brdfIndices[] = {
			0, 1, 2,
			2, 3, 0,
		};
		s_data3d.brdfVAO = VertexArray::Create();
		s_data3d.brdfVAO->Bind();
		s_data3d.brdfVBO = VertexBuffer::Create(brdfData, sizeof(brdfData));
		s_data3d.brdfVBO->SetLayout({
			{ "a_position", ShaderDataType::Float3 },
			{ "a_texcoord", ShaderDataType::Float2 },
		});
		s_data3d.brdfVAO->AddVertexBuffer(s_data3d.brdfVBO);
		s_data3d.brdfIBO = IndexBuffer::Create(brdfIndices, sizeof(brdfIndices));
		s_data3d.brdfVAO->SetIndexBuffer(s_data3d.brdfIBO);
		s_data3d.shaderBRDF->Bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// take a look
		float quadData[] = {
			-0.5f, -0.5f, +0.0f,   1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 0.0f,    10.0f,    1.0f, -1.0f,
			+0.5f, -0.5f, +0.0f,   1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 0.0f,    10.0f,    1.0f, -1.0f,
			+0.5f, +0.5f, +0.0f,   1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 1.0f,    10.0f,    1.0f, -1.0f,
			-0.5f, +0.5f, +0.0f,   1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 1.0f,    10.0f,    1.0f, -1.0f,
		};
		s_data3d.talVAO = VertexArray::Create();
		s_data3d.talVBO = VertexBuffer::Create(quadData, sizeof(quadData));
		s_data3d.talIBO = IndexBuffer::Create(brdfIndices, sizeof(brdfIndices));
		s_data3d.talVBO->SetLayout({
			{"a_position", ShaderDataType::Float3},
			{"a_color", ShaderDataType::Float4},
			{"a_texcoord", ShaderDataType::Float2},
			{"a_texIndex", ShaderDataType::Float},
			{"a_tilingFactor", ShaderDataType::Float},
			{"a_entityId", ShaderDataType::Int},
		});
		s_data3d.talVAO->AddVertexBuffer(s_data3d.talVBO);
		s_data3d.talVAO->SetIndexBuffer(s_data3d.talIBO);
		s_data3d.cameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::CameraData), 0);
	}

	void Neko::Renderer3D::BeginScene(const EditorCamera& camera) {
		glm::mat4 viewProjection = camera.GetMatrix();

		s_data3d.shaderModel->Bind();
		s_data3d.shaderModel->SetMat4("u_viewProjection", viewProjection);
		s_data3d.shaderModel->SetVec3("u_cameraPos", camera.GetPosition());

		// temp
		s_data3d.shaderModel->SetFloat("u_metallic", 0.999f);
		s_data3d.shaderModel->SetFloat("u_roughness", 0.04f);
		s_data3d.shaderModel->SetFloat("u_ao", 1.0f);
		s_data3d.shaderModel->SetInt("u_iradianceMap", 0);
		s_data3d.shaderModel->SetInt("u_prefliterMap", 1);
		s_data3d.shaderModel->SetInt("u_brdfLUT", 2);
		glBindTextureUnit(0, s_data3d.iradianceCubeMap);
		BindDepthTexture(8);

		glm::mat4 backgroundVPMatrix = camera.GetProjection() * glm::mat4(glm::mat3(camera.GetViewMatrix()));
		s_data3d.shaderCubeMap->Bind();
		s_data3d.shaderCubeMap->SetMat4("u_viewProjection", backgroundVPMatrix);
		glBindTextureUnit(0, s_data3d.iradianceCubeMap);

		s_data3d.talVAO->Bind();
		s_data3d.shaderQuad->Bind();
		s_data3d.cameraBuffer.viewProjection = camera.GetMatrix();
		s_data3d.cameraUniformBuffer->SetData(&s_data3d.cameraBuffer, sizeof(Renderer3DData::CameraData));
		glBindTextureUnit(10, s_data3d.brdfLUTTexture);
		// s_data3d.textureHdr->Bind(10);
		// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		

		/*s_data3d.shaderHdr->Bind();
		s_data3d.shaderHdr->SetMat4("u_view", camera.GetViewMatrix());
		s_data3d.shaderHdr->SetMat4("u_projection", camera.GetProjection());
		s_data3d.shaderHdr->SetInt("u_equirectangularMap", 1);
		s_data3d.textureHdr->Bind(1);

		s_data3d.showHdrCube->Draw(glm::mat4(1.0f), s_data3d.shaderHdr, -1);*/
		glBindTextureUnit(1, s_data3d.prefliterMap);
		glBindTextureUnit(2, s_data3d.brdfLUTTexture);
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
		s_data3d.shaderCubeMap->SetInt("u_cubemap", 0);
		glBindTextureUnit(0, s_data3d.envCubemap);
		// glBindTextureUnit(0, s_data3d.iradianceCubeMap);
		// glBindTextureUnit(0, s_data3d.prefliterMap);
		s_data3d.backgroundCube->Draw(glm::mat4(1.0f), s_data3d.shaderCubeMap, -1);
		glDepthFunc(GL_LESS);
	}

}
