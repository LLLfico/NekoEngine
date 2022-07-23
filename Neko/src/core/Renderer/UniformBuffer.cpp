#include "pch.h"
#include "UniformBuffer.h"

#include "Renderer.h"
#include "platform/opengl/OpenGLUniformBuffer.h"

namespace Neko {

	std::shared_ptr<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding) {
		switch (Renderer::GetAPI()) {
			case RendererAPI::API::None: {
				NEKO_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
				return nullptr;
			}
			case RendererAPI::API::OpenGL: {
				return std::make_shared<OpenGLUniformBuffer>(size, binding);
			}
		}

		NEKO_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}