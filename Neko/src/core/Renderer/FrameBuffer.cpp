#include "pch.h"
#include "FrameBuffer.h"

#include "Renderer.h"
#include "platform/opengl/OpenGLFrameBuffer.h"

namespace Neko {

	std::shared_ptr<FrameBuffer> FrameBuffer::Create(const FrameBufferDesc& desc) {
		auto api = Renderer::GetAPI();
		switch (api) {
		case RendererAPI::API::None: {
			NEKO_ASSERT(false, "RenderAPI::None is currently not supported!");
			return nullptr;
		}
		case RendererAPI::API::OpenGL: {
			return std::make_shared<OpenGLFrameBuffer>(desc);
		}
		}
		NEKO_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}