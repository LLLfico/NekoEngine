#include "pch.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace Neko {

	static const uint32_t s_maxFrameBufferSize = 8192;

	namespace Utils {
		static GLenum TextureTarget(bool multisampled) {
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void CreateTextures(bool multisampled, uint32_t count, uint32_t* outIds) {
			glCreateTextures(TextureTarget(multisampled), count, outIds);
		}

		static void BindTexture(bool multisampled, uint32_t id) {
			glBindTexture(TextureTarget(multisampled), id);
		}

		static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index) {
			bool multisampled = samples > 1;
			if (multisampled) {
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else {
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		}

		static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height) {
			bool multisampled = samples > 1;
			if (multisampled) {
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else {
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
		}

		static bool IsDepthFormat(FrameBufferTextureFormat format) {
			switch (format) {
			case FrameBufferTextureFormat::DEPTH24STENCIL8: return true;
			}
			return false;
		}

		static GLenum SwitchToGLTextureFormat(FrameBufferTextureFormat format) {
			switch (format) {
				case FrameBufferTextureFormat::RGBA8: return GL_RGBA8;
				case FrameBufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
			}
			NEKO_CORE_ASSERT(false, "");
			return 0;
		}
	}

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferDesc& desc) : m_desc(desc) {
		for (auto desc : m_desc.attachments.attachments) {
			if (Utils::IsDepthFormat(desc.format)) {
				m_depthAttachmentDesc = desc;
			}
			else {
				m_colorAttachmentsDesc.emplace_back(desc);
			}

		}
		Invalidate();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer() {
		glDeleteFramebuffers(1, &m_id);
		glDeleteTextures(m_colorAttachments.size(), m_colorAttachments.data());
		glDeleteTextures(1, &m_depthAttachment);
	}

	void OpenGLFrameBuffer::Invalidate() {
		if (m_id) {
			glDeleteFramebuffers(1, &m_id);
			glDeleteTextures(m_colorAttachments.size(), m_colorAttachments.data());
			glDeleteTextures(1, &m_depthAttachment);
		}

		glCreateFramebuffers(1, &m_id);
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);

		bool multisample = m_desc.samples > 1;

		if (m_colorAttachmentsDesc.size()) {
			m_colorAttachments.resize(m_colorAttachmentsDesc.size());
			Utils::CreateTextures(multisample, m_colorAttachments.size(), m_colorAttachments.data());

			for (size_t i = 0; i < m_colorAttachments.size(); i++) {
				Utils::BindTexture(multisample, m_colorAttachments[i]);
				switch (m_colorAttachmentsDesc[i].format) {
					case FrameBufferTextureFormat::RGBA8: {
						Utils::AttachColorTexture(m_colorAttachments[i], m_desc.samples, GL_RGBA8, GL_RGBA, m_desc.width, m_desc.height, i);
						break;
					}
					case FrameBufferTextureFormat::RED_INTEGER: {
						Utils::AttachColorTexture(m_colorAttachments[i], m_desc.samples, GL_R32I, GL_RED_INTEGER, m_desc.width, m_desc.height, i);
					}
				}
			}
		}
		if (m_depthAttachmentDesc.format != FrameBufferTextureFormat::None) {
			Utils::CreateTextures(multisample, 1, &m_depthAttachment);
			Utils::BindTexture(multisample, m_depthAttachment);
			switch (m_depthAttachmentDesc.format) {
				case FrameBufferTextureFormat::DEPTH24STENCIL8: {
					Utils::AttachDepthTexture(m_depthAttachment, m_desc.samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_desc.width, m_desc.height);
					break;
				}
			}
		}
		if (m_colorAttachments.size() > 1) {
			NEKO_CORE_ASSERT(m_colorAttachments.size() < 4, "");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_colorAttachments.size(), buffers);
		}
		else if (m_colorAttachments.empty()) {
			// depth frame 
			glDrawBuffer(GL_NONE);
		}
		auto flag = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		NEKO_CORE_ASSERT(flag == GL_FRAMEBUFFER_COMPLETE, "Framebuffer initializing failed!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);
		glViewport(0, 0, m_desc.width, m_desc.height);
	}

	void OpenGLFrameBuffer::Unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height) {
		m_desc.width = width;
		m_desc.height = height;

		Invalidate();
	}

	int OpenGLFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y) {
		NEKO_CORE_ASSERT(attachmentIndex < m_colorAttachments.size(), "");

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		return pixelData;
	}

	void OpenGLFrameBuffer::ClearAttachment(uint32_t index, int value) {
		NEKO_CORE_ASSERT(index < m_colorAttachments.size(), "");

		auto& desc = m_colorAttachmentsDesc[index];
		glClearTexImage(m_colorAttachments[index], 0, Utils::SwitchToGLTextureFormat(desc.format), GL_INT, &value);

	}

}