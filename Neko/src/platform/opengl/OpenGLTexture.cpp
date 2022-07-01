#include "pch.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace Neko {

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : m_path(path) {
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		NEKO_CORE_ASSERT(data, "Failed to load image!");
		m_width = width;
		m_height = height;

		GLenum innerFormat, format;
		if (channels == 4) {
			innerFormat = GL_RGBA8;
			format = GL_RGBA;
		}
		else if (channels == 3) {
			innerFormat = GL_RGB8;
			format = GL_RGB;
		}
		else {
			NEKO_ASSERT(false, "Unknown Image Channel {0}!", channels);
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
		glTextureStorage2D(m_id, 1, innerFormat, m_width, m_height);

		glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_id, 0, 0, 0, m_width, m_height, format, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D() {
		glDeleteTextures(1, &m_id);
	}
	uint32_t OpenGLTexture2D::GetWidth() const {
		return uint32_t();
	}
	uint32_t OpenGLTexture2D::GetHeight() const {
		return uint32_t();
	}
	void OpenGLTexture2D::Bind(uint32_t slot) const {
		glBindTextureUnit(slot, m_id);
	}
}