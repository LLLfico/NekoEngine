#pragma once
#include "core/Core.h"

#include <glm/glm.hpp>

namespace Neko {

	class VertexArray;

	class NEKO_API RendererAPI {
	public:
		enum class API {
			None,
			OpenGL,
		};

	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawElement(const std::shared_ptr<VertexArray>& vertexArray, uint32_t count = 0) = 0;
		virtual void DrawLines(const std::shared_ptr<VertexArray>& vertexArray, uint32_t count = 0) = 0;
		virtual void SetLineWidth(float width) = 0;
		virtual void DrawArrays(uint32_t size) = 0;

		virtual void BindDrawFramebuffer() = 0;

		static API GetAPI() { return s_api; }
	private:
		static API s_api;
	};

}