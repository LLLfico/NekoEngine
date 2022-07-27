#pragma once

#include "Camera.h"

#include "Projection.h"
#include "EditorCamera.h"

namespace Neko {

	class Texture2D;
	struct SpriteRendererComponent;
	class NEKO_API Renderer2D {
	public:
		static void Init();
		static void Shutdown();
		
		static void BeginScene(const Camera& camera);
		static void BeginScene(const Projection& projection, const glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);
		static void EndScene();
		static void Flush();

		static void DrawQuad(const glm::vec2& position, const glm::vec2& scale, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color);
		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityId = -1);

		static void DrawQuad(const glm::vec2& position, const glm::vec2& scale, const std::shared_ptr<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& scale, const std::shared_ptr<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), int entityId = -1);

		static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickiness = 1.0f, float fade = 0.005f, int entityId = -1);

		static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityId = -1);
		static void DrawRect(const glm::vec3& position, const glm::vec3& scale, const glm::vec4& color, int entityId = -1);
		static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityId = -1);

		static void DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityId);
		static void DrawQuadTemp(uint32_t texture);

		static float GetLineWidth();
		static void SetLineWidth(float width);

		struct Statistics {
			uint32_t drawCalls = 0;
			uint32_t quadCount = 0;

			uint32_t GetTotalVertexCount() const { return quadCount * 4; }
			uint32_t GetTotalIndexCount() const { return quadCount * 6; }
		};

		static void ResetStatistics();
		static Statistics GetStatistics();
	private:
		static void StartBatch();
		static void NextBatch();
	};

}