#pragma once

#include "core/renderer/FrameBuffer.h"

namespace Neko {

	class OpenGLFrameBuffer : public FrameBuffer {
	public:
		OpenGLFrameBuffer(const FrameBufferDesc& desc);
		virtual ~OpenGLFrameBuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		virtual void ClearAttachment(uint32_t index, int value) override;

		virtual uint32_t GetColorAttachmentId(uint32_t index) const override { NEKO_CORE_ASSERT(index < m_colorAttachments.size(), "");  return m_colorAttachments[index]; }

		virtual const FrameBufferDesc& GetDesc() const override { return m_desc; }
	private:
		uint32_t m_id = 0;
		FrameBufferDesc m_desc;

		std::vector<FrameBufferTextureDesc> m_colorAttachmentsDesc;
		FrameBufferTextureDesc m_depthAttachmentDesc = FrameBufferTextureFormat::None;

		std::vector<uint32_t> m_colorAttachments;
		uint32_t m_depthAttachment = 0;
	};

}