#include "pch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "platform/opengl/OpenGLBuffer.h"

namespace Neko {

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size) {
		auto api = Renderer::GetAPI();
		switch (api) {
			case RendererAPI::API::None: {
				NEKO_ASSERT(false, "RenderAPI::None is currently not supported!");
				return nullptr;
			}
			case RendererAPI::API::OpenGL: {
				return std::make_shared<OpenGLVertexBuffer>(vertices, size);
			}
		}
		NEKO_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(uint32_t size) {
		auto api = Renderer::GetAPI();
		switch (api) {
			case RendererAPI::API::None: {
				NEKO_ASSERT(false, "RenderAPI::None is currently not supported!");
				return nullptr;
			}
			case RendererAPI::API::OpenGL: {
				return std::make_shared<OpenGLVertexBuffer>(size);
			}
		}
		NEKO_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	std::shared_ptr<IndexBuffer> IndexBuffer::Create(unsigned int* indices, uint32_t size) {
		auto api = Renderer::GetAPI();
		switch (api) {
			case RendererAPI::API::None: {
				NEKO_ASSERT(false, "RenderAPI::None is currently not supported!");
				return nullptr;
			}
			case RendererAPI::API::OpenGL: {
				return std::make_shared<OpenGLIndexBuffer>(indices, size);
			}
		}
		NEKO_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}