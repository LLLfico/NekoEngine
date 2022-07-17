#include "pch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"
#include "Texture.h"

#include "world/Component.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Neko {

	struct QuadVertex {
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 texcoord;
		float texIndex;
		float tilingFactor;

		int entityId;
	};

	struct Renderer2DData {
		static const uint32_t maxQuads = 20000;
		static const uint32_t maxVertices = maxQuads * 4;
		static const uint32_t maxIndices = maxQuads * 6;
		static const uint32_t maxTextureSlots = 32; // TODO:get from device

		std::shared_ptr<VertexArray> quadVertexArray;
		std::shared_ptr<VertexBuffer> quadVertexBuffer;
		std::shared_ptr<Shader> shader;
		std::shared_ptr<Texture2D> whiteTexture;

		uint32_t quadIndexCount = 0;					// current num of index 
		QuadVertex* quadVertexBufferHead = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr;

		std::array<std::shared_ptr<Texture2D>, maxTextureSlots> textureSlots;
		uint32_t textureSlotIndex = 1; // 0 is default white texture

		std::array<glm::vec4, 4> quadPoints;

		Renderer2D::Statistics statistics;
	};

	static Renderer2DData s_data;

	void Renderer2D::Init() {
		s_data.quadVertexArray = VertexArray::Create();

		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		};
		uint32_t indices[] = {
			 0, 1, 2,
			 2, 3, 0,
		};

		s_data.quadVertexBuffer = VertexBuffer::Create(s_data.maxVertices * sizeof(QuadVertex));
		s_data.quadVertexBuffer->SetLayout({
			{"a_position", ShaderDataType::Float3},
			{"a_color", ShaderDataType::Float4},
			{"a_texcoord", ShaderDataType::Float2},
			{"a_texIndex", ShaderDataType::Float},
			{"a_tilingFactor", ShaderDataType::Float},
			{"a_entityId", ShaderDataType::Int},
		});
		s_data.quadVertexArray->AddVertexBuffer(s_data.quadVertexBuffer);
		s_data.quadVertexBufferHead = new QuadVertex[s_data.maxVertices];


		uint32_t* quadIndices = new uint32_t[s_data.maxIndices];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_data.maxIndices; i+=6) {
			quadIndices[i + 0] = offset + 0u;
			quadIndices[i + 1] = offset + 1u;
			quadIndices[i + 2] = offset + 2u;

			quadIndices[i + 3] = offset + 2u;
			quadIndices[i + 4] = offset + 3u;
			quadIndices[i + 5] = offset + 0u;

			offset += 4;
		}
		std::shared_ptr<IndexBuffer> ibo = IndexBuffer::Create(quadIndices, s_data.maxIndices);
		s_data.quadVertexArray->SetIndexBuffer(ibo);
		delete[] quadIndices;

		s_data.whiteTexture = Texture2D::Create(1, 1);
		uint32_t white = 0xffffffff;
		s_data.whiteTexture->SetData(&white, sizeof(uint32_t));

		int32_t samplers[s_data.maxTextureSlots]; // use int to match shader's int array
		for (uint32_t i = 0; i < s_data.maxTextureSlots; i++) {
			samplers[i] = i;
		}

		s_data.shader = Shader::Create("assets/shaders/shader2d.glsl");
		s_data.shader->Bind();
		// s_data.shader->SetInt("u_texture", 0);
		s_data.shader->SetIntArray("u_texture", samplers, s_data.maxTextureSlots);

		s_data.textureSlots[0] = s_data.whiteTexture;

		s_data.quadPoints[0] = { -0.5f, -0.5f, +0.0f, 1.0f };
		s_data.quadPoints[1] = { +0.5f, -0.5f, +0.0f, 1.0f };
		s_data.quadPoints[2] = { +0.5f, +0.5f, +0.0f, 1.0f };
		s_data.quadPoints[3] = { -0.5f, +0.5f, +0.0f, 1.0f };
	}

	void Renderer2D::Shutdown() {
	}

	void Renderer2D::BeginScene(const Camera& camera) {
		s_data.shader->Bind();
		s_data.shader->SetMat4("u_viewProjection", camera.GetMatrix());

		s_data.quadIndexCount = 0;
		s_data.quadVertexBufferPtr = s_data.quadVertexBufferHead;

		s_data.textureSlotIndex = 1;
	}

	void Renderer2D::BeginScene(const Projection& projection, const glm::mat4& transform) {
		glm::mat4 viewProjection = projection.GetProjection() * glm::inverse(transform);

		s_data.shader->Bind();
		s_data.shader->SetMat4("u_viewProjection", viewProjection);

		s_data.quadIndexCount = 0;
		s_data.quadVertexBufferPtr = s_data.quadVertexBufferHead;

		s_data.textureSlotIndex = 1;

	}

	void Renderer2D::BeginScene(const EditorCamera& camera) {
		glm::mat4 viewProjection = camera.GetMatrix();

		s_data.shader->Bind();
		s_data.shader->SetMat4("u_viewProjection", viewProjection);

		s_data.quadIndexCount = 0;
		s_data.quadVertexBufferPtr = s_data.quadVertexBufferHead;

		s_data.textureSlotIndex = 1;
	}

	void Renderer2D::EndScene() {
		uint32_t dataSize = (uint8_t*)s_data.quadVertexBufferPtr - (uint8_t*)s_data.quadVertexBufferHead;
		s_data.quadVertexBuffer->SetData(s_data.quadVertexBufferHead, dataSize);
		Flush();
	}

	void Renderer2D::Flush() {
		for (uint32_t i = 0; i < s_data.textureSlotIndex; i++) {
			s_data.textureSlots[i]->Bind(i);
		}

		RenderCommand::DrawElement(s_data.quadVertexArray, s_data.quadIndexCount);
		s_data.statistics.drawCalls++;
	}

	void Renderer2D::FlushAndReset() {
		EndScene();
		s_data.quadIndexCount = 0;
		s_data.quadVertexBufferPtr = s_data.quadVertexBufferHead;

		s_data.textureSlotIndex = 1;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& scale, const glm::vec4& color) {
		DrawQuad({ position.x, position.y, 0.0f }, scale, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color) {

		if (s_data.quadIndexCount >= s_data.maxIndices)
			FlushAndReset();

		const float tilingFactor = 1.0f;
		auto transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityId) {
		if (s_data.quadIndexCount >= s_data.maxIndices)
			FlushAndReset();
		constexpr size_t quadVertexCount = 4;
		constexpr float whiteTextureIndex = 0;
		constexpr glm::vec2 texcoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };
		constexpr float tilingFactor = 1.0f;

		for (size_t i = 0; i < quadVertexCount; i++) {
			s_data.quadVertexBufferPtr->position = transform * s_data.quadPoints[i];
			s_data.quadVertexBufferPtr->color = color;
			s_data.quadVertexBufferPtr->texcoord = texcoords[i];
			s_data.quadVertexBufferPtr->texIndex = whiteTextureIndex;
			s_data.quadVertexBufferPtr->tilingFactor = tilingFactor;
			s_data.quadVertexBufferPtr->entityId = entityId;
			s_data.quadVertexBufferPtr++;
		}

		s_data.quadIndexCount += 6;

		s_data.statistics.quadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& scale, const std::shared_ptr<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor) {
		DrawQuad({ position.x, position.y, 0.0f }, scale, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& scale, const std::shared_ptr<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor) {
		auto transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);

		//s_data.shader->SetVec4("u_color", tintColor);
		//s_data.shader->SetFloat("u_tilingFactor", tilingFactor);
		//auto transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });
		//s_data.shader->SetMat4("u_transform", transform);


		//texture->Bind(0);
		//s_data.quadVertexArray->Bind();
		//RenderCommand::DrawElement(s_data.quadVertexArray);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, int entityId) {
		if (s_data.quadIndexCount >= s_data.maxIndices)
			FlushAndReset();

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		constexpr glm::vec2 texcoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_data.textureSlotIndex; i++) {
			if (*s_data.textureSlots[i].get() == *texture.get()) {
				textureIndex = (float)i;
				break;
			}
		}
		if (textureIndex == 0.0f) {
			textureIndex = (float)s_data.textureSlotIndex;
			s_data.textureSlots[s_data.textureSlotIndex] = texture;
			s_data.textureSlotIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++) {
			s_data.quadVertexBufferPtr->position = transform * s_data.quadPoints[i];
			s_data.quadVertexBufferPtr->color = color;
			s_data.quadVertexBufferPtr->texcoord = texcoords[i];
			s_data.quadVertexBufferPtr->texIndex = textureIndex;
			s_data.quadVertexBufferPtr->tilingFactor = tilingFactor;
			s_data.quadVertexBufferPtr->entityId = entityId;
			s_data.quadVertexBufferPtr++;
		}

		s_data.quadIndexCount += 6;

		s_data.statistics.quadCount++;
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityId) {
		DrawQuad(transform, src.color, entityId);
	}

	void Renderer2D::ResetStatistics() {
		memset(&s_data.statistics, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStatistics() {
		return s_data.statistics;
	}

}