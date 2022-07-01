#pragma once

#include "core/Renderer/RendererAPI.h"

#include <glm/glm.hpp>

namespace Neko {

	class OpenGLRendererAPI : public RendererAPI {
	public:
		virtual void Init() override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
		virtual void DrawElement(const std::shared_ptr<VertexArray>& vertexArray) override;
	};

}