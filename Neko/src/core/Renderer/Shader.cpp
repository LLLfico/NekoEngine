#include "pch.h"
#include "Shader.h"
#include "Renderer.h"

#include "platform/opengl/OpenGLShader.h"

#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

namespace Neko {

	std::shared_ptr<Shader> Shader::Create(const std::string& vertexDesc, const std::string& fragDesc) {
		switch (Renderer::GetAPI()) {
			case RendererAPI::API::None : {
				NEKO_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
				return nullptr;
			}
			case RendererAPI::API::OpenGL: {
				return std::make_shared<OpenGLShader>(vertexDesc, fragDesc);
			}
		}

		NEKO_CORE_ASSERT(false, "Unknown RendererAPI!");
		return std::shared_ptr<Shader>();
	}

}
