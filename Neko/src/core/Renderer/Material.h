#pragma once

#include "Texture.h"

#include <glm/glm.hpp>

namespace Neko {

	enum class TextureType {
		Albedo = 0,
		Normal,
	};

	struct MaterialTexture {
		std::shared_ptr<Texture2D> texture;
		TextureType type;
		std::string path;
	};

	class Material {
	public:
		Material() = default;

		void SetAlbedoMap(std::shared_ptr<Texture2D> texture);
		void AddTexture(MaterialTexture texture) { m_textures.push_back(texture); }

		const std::vector<MaterialTexture>& GetTextures() const { return m_textures; }
		size_t GetTexturesNum() const { return m_textures.size(); }

	public:
		bool m_useAlbedo = false;
		glm::vec4 m_color = glm::vec4(0.5f, 0.3f, 0.0f, 1.0f);
		std::shared_ptr<Texture2D> m_albedoColor = Texture2D::Create(1, 1);
		std::shared_ptr<Texture2D> m_albedoMap;
	private:
		std::vector<MaterialTexture> m_textures;
	};

}