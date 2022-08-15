#pragma once

#include "Texture.h"
#include "TextureLibrary.h"

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
		bool m_useAlbedoMap = false;
		glm::vec4 m_color = glm::vec4(0.5f, 0.3f, 0.0f, 1.0f);
		std::shared_ptr<Texture2D> m_albedoSprite = Texture2D::Create(1, 1);
		std::shared_ptr<Texture2D> m_albedoMap = TextureLibrary::GetInstance().Get("White");

		bool m_useNormalMap = false;
		std::shared_ptr<Texture2D> m_normalMap = TextureLibrary::GetInstance().Get("DefaultNormal");

		bool m_useMetallicMap = false;
		float m_metalllic = 0.8f;
		std::shared_ptr<Texture2D> m_metallicSprite = Texture2D::Create(1, 1);
		std::shared_ptr<Texture2D> m_metallicMap = TextureLibrary::GetInstance().Get("DefaultMetallic");

		bool m_useRoughnessMap = false;
		float m_roughness = 0.4f;
		std::shared_ptr<Texture2D> m_roughnessSprite = Texture2D::Create(1, 1);
		std::shared_ptr<Texture2D> m_roughnessMap = TextureLibrary::GetInstance().Get("DefaultRoughness");

		bool m_useAOMap = false;
		float m_ao = 1.0f;
		std::shared_ptr<Texture2D> m_aoSprite = Texture2D::Create(1, 1);
		std::shared_ptr<Texture2D> m_aoMap = TextureLibrary::GetInstance().Get("White");

	private:
		std::vector<MaterialTexture> m_textures;
	};

}