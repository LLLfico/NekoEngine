#include "pch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "RendererAPI.h"
#include "platform/opengl/OpenGLVertexArray.h"

namespace Neko {

	std::shared_ptr<VertexArray> VertexArray::Create() {
		switch (Renderer::GetAPI()) {
			case RendererAPI::API::None: {
				NEKO_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); 
				return nullptr;
			}
			case RendererAPI::API::OpenGL: {
				return std::make_shared<OpenGLVertexArray>();
			}
		}
		NEKO_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}