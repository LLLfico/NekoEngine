#include "pch.h"
#include "RenderCommand.h"

#include "platform/opengl/OpenGLRendererAPI.h"

#include <glm/glm.hpp>

namespace Neko {

	RendererAPI* RenderCommand::s_api = new OpenGLRendererAPI();

}