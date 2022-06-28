#include "pch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "platform/opengl/OpenGLBuffer.h"

namespace Neko {

	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size) {
		auto api = Renderer::GetAPI();
		switch (api) {
			case RendererAPI::None: {
				NEKO_ASSERT(false, "RenderAPI::None is currently not supported!");
				return nullptr;
			}
			case RendererAPI::OpenGL: {
				return new OpenGLVertexBuffer(vertices, size);
			}
		}
		NEKO_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(unsigned int* indices, uint32_t size) {
		auto api = Renderer::GetAPI();
		switch (api) {
			case RendererAPI::None: {
				NEKO_ASSERT(false, "RenderAPI::None is currently not supported!");
				return nullptr;
			}
			case RendererAPI::OpenGL: {
				return new OpenGLIndexBuffer(indices, size);
			}
		}
		NEKO_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}