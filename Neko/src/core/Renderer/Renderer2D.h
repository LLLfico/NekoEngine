#pragma once

#include "Camera.h"

namespace Neko {

	class NEKO_API Renderer2D {
	public:
		static void Init();
		static void Shutdown();
		
		static void BeginScene(const Camera& camera);
		static void EndScene();

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
	};

}