#include "pch.h"
#include "OpenGLRendererAPI.h"

#include "core/renderer/VertexArray.h"
#include "core/renderer/Buffer.h"

#include <glad/glad.h>

namespace Neko {
	void OpenGLRendererAPI::Init() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// blend and depth test somewhere confilct, need to deal
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
	}
	void OpenGLRendererAPI::SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color) {
		glClearColor(color.r, color.g, color.b, color.a);
	}
	void OpenGLRendererAPI::Clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void OpenGLRendererAPI::DrawElement(const std::shared_ptr<VertexArray>& vertexArray, uint32_t count) {
		vertexArray->Bind();
		uint32_t cnt = count <= 0 ? vertexArray->GetIndexBuffer()->Count() : count;
		glDrawElements(GL_TRIANGLES, cnt, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawLines(const std::shared_ptr<VertexArray>& vertexArray, uint32_t count) {
		vertexArray->Bind();
		glDrawArrays(GL_LINES, 0, count);
	}

	void OpenGLRendererAPI::SetLineWidth(float width) {
		glLineWidth(width);
	}

	void OpenGLRendererAPI::DrawArrays(uint32_t size) {
		glDrawArrays(GL_TRIANGLES, 0, size);
	}

}