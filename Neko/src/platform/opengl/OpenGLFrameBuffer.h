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

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual uint32_t GetColorAttachmentId() const override { return m_colorAttachment; }

		virtual const FrameBufferDesc& GetDesc() const override { return m_desc; }
	private:
		uint32_t m_id = 0;
		uint32_t m_colorAttachment = 0, m_depthAttachment = 0;
		FrameBufferDesc m_desc;
	};

}