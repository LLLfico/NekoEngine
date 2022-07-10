#pragma once
#include "core/Core.h"

namespace Neko {

	struct FrameBufferDesc {
		uint32_t width, height;
		uint32_t samples = 1;

		bool swapChainTarget = false;
	};

	class NEKO_API FrameBuffer {
	public:
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetColorAttachmentId() const = 0;

		virtual const FrameBufferDesc& GetDesc() const = 0;

		static std::shared_ptr<FrameBuffer> Create(const FrameBufferDesc& desc);
	};

}