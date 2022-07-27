#include "pch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"
#include "Texture.h"
#include "UniformBuffer.h"

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

	struct CircleVertex {
		glm::vec3 worldPosition;
		glm::vec3 localPosition;
		glm::vec4 color;
		float thickness;
		float fade;

		int entityId;
	};

	struct LineVertex {
		glm::vec3 position;
		glm::vec4 color;

		int entityId;
	};

	struct Renderer2DData {
		static const uint32_t maxQuads = 20000;
		static const uint32_t maxVertices = maxQuads * 4;
		static const uint32_t maxIndices = maxQuads * 6;
		static const uint32_t maxTextureSlots = 32; // TODO:get from device

		std::shared_ptr<VertexArray> quadVertexArray;
		std::shared_ptr<VertexBuffer> quadVertexBuffer;
		std::shared_ptr<Shader> quadShader;
		std::shared_ptr<Texture2D> whiteTexture;

		std::shared_ptr<VertexArray> circleVertexArray;
		std::shared_ptr<VertexBuffer> circleVertexBuffer;
		std::shared_ptr<Shader> circleShader;

		std::shared_ptr<VertexArray> lineVertexArray;
		std::shared_ptr<VertexBuffer> lineVertexBuffer;
		std::shared_ptr<Shader> lineShader;

		uint32_t quadIndexCount = 0;					// current num of index 
		QuadVertex* quadVertexBufferHead = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr;

		uint32_t circleIndexCount = 0;
		CircleVertex* circleVertexBufferHead = nullptr;
		CircleVertex* circleVertexBufferPtr = nullptr;

		uint32_t lineIndexCount = 0;
		LineVertex* lineVertexBufferHead = nullptr;
		LineVertex* lineVertexBufferPtr = nullptr;
		float lineWidth = 2.0f;

		std::array<std::shared_ptr<Texture2D>, maxTextureSlots> textureSlots;
		uint32_t textureSlotIndex = 1; // 0 is default white texture

		std::array<glm::vec4, 4> quadPoints;

		Renderer2D::Statistics statistics;

		struct CameraData {
			glm::mat4 viewProjection;
		};
		CameraData cameraBuffer;
		std::shared_ptr<UniformBuffer> cameraUniformBuffer;
	};

	static Renderer2DData s_data;

	void Renderer2D::Init() {
		s_data.quadVertexArray = VertexArray::Create();

		float vertices[] = {
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f,   1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f,   0.0f, 1.0f,
		};
		uint32_t indices[] = {
			 0, 1, 2,
			 2, 3, 0,
		};

		// quads
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

		// circles
		s_data.circleVertexArray = VertexArray::Create();
		s_data.circleVertexBuffer = VertexBuffer::Create(s_data.maxVertices * sizeof(CircleVertex));
		s_data.circleVertexBuffer->SetLayout({
			{ "a_worldPosition", ShaderDataType::Float3 },
			{ "a_localPosition", ShaderDataType::Float3 },
			{ "a_color",         ShaderDataType::Float4 },
			{ "a_thickness",     ShaderDataType::Float  },
			{ "a_fade",          ShaderDataType::Float  },
			{ "a_entityId",      ShaderDataType::Int    },
		});
		s_data.circleVertexArray->AddVertexBuffer(s_data.circleVertexBuffer);
		s_data.circleVertexArray->SetIndexBuffer(ibo);
		s_data.circleVertexBufferHead = new CircleVertex[s_data.maxVertices];

		// lines
		s_data.lineVertexArray = VertexArray::Create();
		s_data.lineVertexBuffer = VertexBuffer::Create(s_data.maxVertices * sizeof(LineVertex));
		s_data.lineVertexBuffer->SetLayout({
			{ "a_position", ShaderDataType::Float3 },
			{ "a_color",    ShaderDataType::Float4 },
			{ "a_entityId", ShaderDataType::Int    },
		});
		s_data.lineVertexArray->AddVertexBuffer(s_data.lineVertexBuffer);
		s_data.lineVertexBufferHead = new LineVertex[s_data.maxVertices];

		s_data.whiteTexture = Texture2D::Create(1, 1);
		uint32_t white = 0xffffffff;
		s_data.whiteTexture->SetData(&white, sizeof(uint32_t));

		int32_t samplers[s_data.maxTextureSlots]; // use int to match shader's int array
		for (uint32_t i = 0; i < s_data.maxTextureSlots; i++) {
			samplers[i] = i;
		}

		s_data.quadShader = Shader::Create("assets/shaders/ShaderQuad.glsl");
		s_data.circleShader = Shader::Create("assets/shaders/ShaderCircle.glsl");
		s_data.lineShader = Shader::Create("assets/shaders/ShaderLine.glsl");
		s_data.quadShader->Bind();
		// s_data.shader->SetInt("u_texture", 0);
		s_data.quadShader->SetIntArray("u_texture", samplers, s_data.maxTextureSlots);

		s_data.textureSlots[0] = s_data.whiteTexture;

		s_data.quadPoints[0] = { -0.5f, -0.5f, +0.0f, 1.0f };
		s_data.quadPoints[1] = { +0.5f, -0.5f, +0.0f, 1.0f };
		s_data.quadPoints[2] = { +0.5f, +0.5f, +0.0f, 1.0f };
		s_data.quadPoints[3] = { -0.5f, +0.5f, +0.0f, 1.0f };

		s_data.cameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
	}

	void Renderer2D::Shutdown() {
		delete[] s_data.quadVertexBufferHead;
	}

	void Renderer2D::BeginScene(const Camera& camera) {
		s_data.cameraBuffer.viewProjection = camera.GetMatrix();
		s_data.cameraUniformBuffer->SetData(&s_data.cameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginScene(const Projection& projection, const glm::mat4& transform) {
		glm::mat4 viewProjection = projection.GetProjection() * glm::inverse(transform);

		s_data.cameraBuffer.viewProjection = viewProjection;
		s_data.cameraUniformBuffer->SetData(&s_data.cameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();

	}

	void Renderer2D::BeginScene(const EditorCamera& camera) {
		s_data.cameraBuffer.viewProjection = camera.GetMatrix();
		s_data.cameraUniformBuffer->SetData(&s_data.cameraBuffer, sizeof(Renderer2DData::CameraData));
		StartBatch();
	}

	void Renderer2D::EndScene() {
		Flush();
	}

	void Renderer2D::Flush() {
		if (s_data.quadIndexCount) {
			uint32_t dataSize = (uint32_t)((uint8_t*)s_data.quadVertexBufferPtr - (uint8_t*)s_data.quadVertexBufferHead);
			s_data.quadVertexBuffer->SetData(s_data.quadVertexBufferHead, dataSize);

			for (uint32_t i = 0; i < s_data.textureSlotIndex; i++) {
				s_data.textureSlots[i]->Bind(i);
			}

			s_data.quadShader->Bind();
			RenderCommand::DrawElement(s_data.quadVertexArray, s_data.quadIndexCount);
			s_data.statistics.drawCalls++;
		}
		if (s_data.circleIndexCount) {
			uint32_t dataSize = (uint32_t)((uint8_t*)s_data.circleVertexBufferPtr - (uint8_t*)s_data.circleVertexBufferHead);
			s_data.circleVertexBuffer->SetData(s_data.circleVertexBufferHead, dataSize);

			s_data.circleShader->Bind();
			RenderCommand::DrawElement(s_data.circleVertexArray, s_data.circleIndexCount);
			s_data.statistics.drawCalls++;
		}
		if (s_data.lineIndexCount) {
			uint32_t dataSize = (uint32_t)((uint8_t*)s_data.lineVertexBufferPtr - (uint8_t*)s_data.lineVertexBufferHead);
			s_data.lineVertexBuffer->SetData(s_data.lineVertexBufferHead, dataSize);

			s_data.lineShader->Bind();
			RenderCommand::SetLineWidth(s_data.lineWidth);
			RenderCommand::DrawLines(s_data.lineVertexArray, s_data.lineIndexCount);
			s_data.statistics.drawCalls++;
		}
	}

	void Renderer2D::StartBatch() {
		s_data.quadIndexCount = 0;
		s_data.quadVertexBufferPtr = s_data.quadVertexBufferHead;

		s_data.circleIndexCount = 0;
		s_data.circleVertexBufferPtr = s_data.circleVertexBufferHead;

		s_data.lineIndexCount = 0;
		s_data.lineVertexBufferPtr = s_data.lineVertexBufferHead;

		s_data.textureSlotIndex = 1;
	}

	void Renderer2D::NextBatch() {
		EndScene();
		StartBatch();
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& scale, const glm::vec4& color) {
		DrawQuad({ position.x, position.y, 0.0f }, scale, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color) {

		if (s_data.quadIndexCount >= s_data.maxIndices)
			NextBatch();

		const float tilingFactor = 1.0f;
		auto transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityId) {
		if (s_data.quadIndexCount >= s_data.maxIndices)
			NextBatch();
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
			NextBatch();

		constexpr size_t quadVertexCount = 4;
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
			s_data.quadVertexBufferPtr->color = tintColor;
			s_data.quadVertexBufferPtr->texcoord = texcoords[i];
			s_data.quadVertexBufferPtr->texIndex = textureIndex;
			s_data.quadVertexBufferPtr->tilingFactor = tilingFactor;
			s_data.quadVertexBufferPtr->entityId = entityId;
			s_data.quadVertexBufferPtr++;
		}

		s_data.quadIndexCount += 6;

		s_data.statistics.quadCount++;
	}

	void Renderer2D::DrawQuadTemp(uint32_t texture) {
		StartBatch();
		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 texcoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };
		glm::vec4 tintColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		for (size_t i = 0; i < quadVertexCount; i++) {
			s_data.quadVertexBufferPtr->position = s_data.quadPoints[i];
			s_data.quadVertexBufferPtr->color = tintColor;
			s_data.quadVertexBufferPtr->texcoord = texcoords[i];
			s_data.quadVertexBufferPtr->texIndex = texture;
			s_data.quadVertexBufferPtr->tilingFactor = 1.0f;
			s_data.quadVertexBufferPtr->entityId = -1;
			s_data.quadVertexBufferPtr++;
		}

		s_data.quadIndexCount += 6;

		s_data.statistics.quadCount++;
		uint32_t dataSize = (uint32_t)((uint8_t*)s_data.quadVertexBufferPtr - (uint8_t*)s_data.quadVertexBufferHead);
		s_data.quadVertexBuffer->SetData(s_data.quadVertexBufferHead, dataSize);

		RenderCommand::DrawElement(s_data.quadVertexArray, s_data.quadIndexCount);
		s_data.statistics.drawCalls++;
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickiness, float fade, int entityId) {
		for (size_t i = 0; i < 4; i++) {
			s_data.circleVertexBufferPtr->worldPosition = transform * s_data.quadPoints[i];
			s_data.circleVertexBufferPtr->localPosition = s_data.quadPoints[i] * 2.0f;
			s_data.circleVertexBufferPtr->color = color;
			s_data.circleVertexBufferPtr->thickness = thickiness;
			s_data.circleVertexBufferPtr->fade = fade;
			s_data.circleVertexBufferPtr->entityId = entityId;
			s_data.circleVertexBufferPtr++;
		}
		s_data.circleIndexCount += 6;
		s_data.statistics.quadCount++;
	}

	void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityId) {
		s_data.lineVertexBufferPtr->position = p0;
		s_data.lineVertexBufferPtr->color = color;
		s_data.lineVertexBufferPtr->entityId = entityId;
		s_data.lineVertexBufferPtr++;

		s_data.lineVertexBufferPtr->position = p1;
		s_data.lineVertexBufferPtr->color = color;
		s_data.lineVertexBufferPtr->entityId = entityId;
		s_data.lineVertexBufferPtr++;

		s_data.lineIndexCount += 2;
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec3& scale, const glm::vec4& color, int entityId) {
		glm::vec3 p0 = glm::vec3(position.x - scale.x * 0.5f, position.y - scale.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + scale.x * 0.5f, position.y - scale.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + scale.x * 0.5f, position.y + scale.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - scale.x * 0.5f, position.y + scale.y * 0.5f, position.z);

		DrawLine(p0, p1, color);
		DrawLine(p1, p2, color);
		DrawLine(p2, p3, color);
		DrawLine(p3, p0, color);
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityId) {
		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; i++) {
			lineVertices[i] = transform * s_data.quadPoints[i];
		}
		DrawLine(lineVertices[0], lineVertices[1], color);
		DrawLine(lineVertices[1], lineVertices[2], color);
		DrawLine(lineVertices[2], lineVertices[3], color);
		DrawLine(lineVertices[3], lineVertices[0], color);
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityId) {
		if (src.texture) {
			DrawQuad(transform, src.texture, src.tilingFactor, src.color, entityId);
		}
		else {
			DrawQuad(transform, src.color, entityId);
		}
	}

	float Renderer2D::GetLineWidth() {
		return s_data.lineWidth;
	}

	void Renderer2D::SetLineWidth(float width) {
		s_data.lineWidth = width;
	}

	void Renderer2D::ResetStatistics() {
		memset(&s_data.statistics, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStatistics() {
		return s_data.statistics;
	}

}