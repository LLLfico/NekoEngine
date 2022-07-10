#pragma once

#include "core/Renderer/FrameBuffer.h"

namespace Neko {

	class OpenGLFrameBuffer : public FrameBuffer {
	public:
		OpenGLFrameBuffer(const FrameBufferDesc& desc);
		virtual ~OpenGLFrameBuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual uint32_t GetColorAttachmentId() const override { return m_colorAttachment; }

		virtual const FrameBufferDesc& GetDesc() const override { return m_desc; }
	private:
		uint32_t m_id;
		uint32_t m_colorAttachment, m_depthAttachment;
		FrameBufferDesc m_desc;
	};

}