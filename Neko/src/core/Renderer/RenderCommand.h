#pragma once
#include "core/Core.h"

#include "RendererAPI.h"


namespace Neko {

	class VertexArray;
	class RenderCommand {
	public:
		inline static void Init() {
			s_api->Init();
		}

		inline static void SetViewPort(uint32_t width, uint32_t height) {
			// glViewport(0, 0, width, height);
			s_api->SetViewPort(0, 0, width, height);
		}

		inline static void SetClearColor(const glm::vec4& color) {
			s_api->SetClearColor(color);
		}

		inline static void Clear() {
			s_api->Clear();
		}

		inline static void DrawElement(const std::shared_ptr<VertexArray>& vertexArray) {
			s_api->DrawElement(vertexArray);
		}

	private:
		static RendererAPI* s_api;
	};

}


