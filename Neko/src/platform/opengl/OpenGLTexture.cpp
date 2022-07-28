#include "pch.h"
#include "OpenGLTexture.h"

#include <stb_image.h>

namespace Neko {

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height) : m_width(width), m_height(height) {
		m_internalFormat = GL_RGBA8;
		m_dataFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
		glTextureStorage2D(m_id, 1, m_internalFormat, m_width, m_height);

		glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : m_path(path) {
		int width = 0, height = 0, channels = 0;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		// NEKO_CORE_ASSERT(data, "Failed to load image!");

		if (!data) return;
		m_isloaded = true;
		m_width = width;
		m_height = height;

		GLenum internalFormat = 0, format = 0;
		if (channels == 4) {
			internalFormat = GL_RGBA8;
			format = GL_RGBA;
		}
		else if (channels == 3) {
			internalFormat = GL_RGB8;
			format = GL_RGB;
		}
		else {
			NEKO_ASSERT(false, "Unknown Image Channel {0}!", channels);
		}
		m_internalFormat = internalFormat;
		m_dataFormat = format;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
		glTextureStorage2D(m_id, 1, internalFormat, m_width, m_height);

		glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_id, 0, 0, 0, m_width, m_height, format, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D() {
		glDeleteTextures(1, &m_id);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size) {
		uint32_t bpp = m_dataFormat == GL_RGBA ? 4 : 3;
		NEKO_CORE_ASSERT(size == m_width * m_height * bpp, "Data size doesnt match!");
		glTextureSubImage2D(m_id, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const {
		glBindTextureUnit(slot, m_id);
	}

	OpenGLTextureCubeMap::OpenGLTextureCubeMap(const std::vector<std::string>& paths) : m_paths(paths) {
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

		int width = 0, height = 0, channels = 0;
		// stbi_set_flip_vertically_on_load(1);
		for (size_t i = 0; i < m_paths.size(); i++) {
			stbi_uc* data = stbi_load(m_paths[i].c_str(), &width, &height, &channels, 0);
			if (!data) continue;
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		m_width = width;
		m_height = height;

		glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	void OpenGLTextureCubeMap::SetData(void* data, uint32_t size) {
	}

	void OpenGLTextureCubeMap::Bind(uint32_t slot) const {
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
	}
}