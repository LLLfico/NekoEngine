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
		virtual void Init() = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawElement(const std::shared_ptr<VertexArray>& vertexArray) = 0;

		inline static API GetAPI() { return s_api; }
	private:
		static API s_api;
	};

}