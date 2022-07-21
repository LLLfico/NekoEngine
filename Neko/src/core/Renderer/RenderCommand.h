#pragma once
#include "core/Core.h"

#include "RendererAPI.h"


namespace Neko {

	class VertexArray;
	class RenderCommand {
	public:
		static void Init() {
			s_api->Init();
		}

		static void SetViewPort(uint32_t width, uint32_t height) {
			// glViewport(0, 0, width, height);
			s_api->SetViewPort(0, 0, width, height);
		}

		static void SetClearColor(const glm::vec4& color) {
			s_api->SetClearColor(color);
		}

		static void Clear() {
			s_api->Clear();
		}

		static void DrawElement(const std::shared_ptr<VertexArray>& vertexArray, uint32_t count = 0) {
			s_api->DrawElement(vertexArray, count);
		}

		static void DrawLines(const std::shared_ptr<VertexArray>& vertexArray, uint32_t count) {
			s_api->DrawLines(vertexArray, count);
		}

		static void SetLineWidth(float width) {
			s_api->SetLineWidth(width);
		}

		static void DrawArrays(uint32_t size) {
			s_api->DrawArrays(size);
		}

	private:
		static std::unique_ptr<RendererAPI> s_api;
	};

}


