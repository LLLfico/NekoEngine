#pragma once
#include "core/Renderer/Texture.h"

namespace Neko {

	class OpenGLTexture2D : public Texture2D {
	public:
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override;
		virtual uint32_t GetHeight() const override;

		virtual void Bind(uint32_t slot = 0) const override;
	private:
		std::string m_path;
		uint32_t m_width, m_height;
		uint32_t m_id;
	};

}