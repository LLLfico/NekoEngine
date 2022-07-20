#pragma once
#include "core/renderer/Texture.h"
#include <glad/glad.h>

namespace Neko {

	class OpenGLTexture2D : public Texture2D {
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height);
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return  m_width; }
		virtual uint32_t GetHeight() const override { return m_height; }
		virtual uint32_t GetId() const override { return m_id; }

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual bool IsLoaded() const override { return m_isloaded; }

		virtual bool operator==(const Texture& tex) const override {
			return m_id == tex.GetId();
		}
	private:
		std::string m_path;
		bool m_isloaded = false;
		uint32_t m_width, m_height;
		uint32_t m_id;
		GLenum m_internalFormat, m_dataFormat;
	};

}