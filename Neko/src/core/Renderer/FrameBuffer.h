#pragma once
#include "core/Core.h"

namespace Neko {

	enum class FrameBufferTextureFormat {
		None = 0,
		// color
		RGBA8,
		RED_INTEGER,
		// depth-stencil
		DEPTH24STENCIL8,
		// defaults
		Depth = DEPTH24STENCIL8,
	};

	struct FrameBufferTextureDesc {
		FrameBufferTextureDesc() = default;
		FrameBufferTextureDesc(FrameBufferTextureFormat _format) : format(_format) {}
		FrameBufferTextureFormat format = FrameBufferTextureFormat::None;
		// filter/ wrap
	};

	struct FrameBufferAttachmentDesc {
		FrameBufferAttachmentDesc() = default;
		FrameBufferAttachmentDesc(std::initializer_list<FrameBufferTextureDesc> _attachments) : attachments(_attachments) {}
		std::vector<FrameBufferTextureDesc> attachments;
	};

	struct FrameBufferDesc {
		uint32_t width, height;
		FrameBufferAttachmentDesc attachments;
		uint32_t samples = 1;

		bool swapChainTarget = false;
	};

	class NEKO_API FrameBuffer {
	public:
		virtual ~FrameBuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual void ClearAttachment(uint32_t index, int value) = 0;

		virtual uint32_t GetColorAttachmentId(uint32_t index) const = 0;

		virtual const FrameBufferDesc& GetDesc() const = 0;

		static std::shared_ptr<FrameBuffer> Create(const FrameBufferDesc& desc);
	};

}