#include "pch.h"
#include "RenderCommand.h"

#include "platform/opengl/OpenGLRendererAPI.h"

#include <glm/glm.hpp>

namespace Neko {

	std::unique_ptr<RendererAPI> RenderCommand::s_api = std::make_unique<OpenGLRendererAPI>();

}