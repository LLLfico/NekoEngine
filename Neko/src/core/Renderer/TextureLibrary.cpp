#include "pch.h"
#include "TextureLibrary.h"

namespace Neko {
	TextureLibrary::TextureLibrary() {
		std::shared_ptr<Texture2D> whiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xFFFFFFFF;
		whiteTexture->SetData(&whiteTextureData, sizeof(whiteTextureData));
		Add("White", whiteTexture);

		std::shared_ptr<Texture2D> blackTexture = Texture2D::Create(1, 1);
		uint32_t blackTextureData = 0x00000000;
		blackTexture->SetData(&blackTextureData, sizeof(blackTextureData));
		Add("Black", blackTexture);

		Add("DefaultTexture", Get("White"));
		Add("DefaultMatallic", Get("Black"));
		Add("DefaultRoughness", Get("Black"));

		Load("DefaultNormal", "assets/textures/default/DefaultNormal.png");
		Load("DefaultMetallic", "assets/textures/default/Black2K.png");
		Load("DefaultRoughness", "assets/textures/default/Black2K.png");

	}

	void TextureLibrary::Add(const std::string& name, const std::shared_ptr<Texture2D> texture) {
		if (!Exist(name)) {
			m_textures[name] = texture;
		}
		else {
			NEKO_CORE_WARN("Texture already exists!");
		}
	}

	std::shared_ptr<Texture2D> TextureLibrary::Load(const std::string& filepath) {
		auto lastSlash = filepath.find_last_of('/');
		auto lastdot = filepath.find_last_of('.');
		if (lastSlash != std::string::npos && lastdot != std::string::npos && lastdot > lastSlash) {
			auto name = filepath.substr(lastSlash + 1, lastdot - lastSlash - 1);
			if (Exist(name)) {
				return m_textures[name];
			}
			auto texture = Texture2D::Create(filepath);
			Add(name, texture);
			return texture;
		}
		else {
			auto name = filepath;
			if (Exist(name)) {
				return m_textures[name];
			}
			auto texture = Texture2D::Create(filepath);
			Add(filepath, texture);
			return texture;
		}
	}

	std::shared_ptr<Texture2D> TextureLibrary::Load(const std::string& name, const std::string& filepath) {
		if (Exist(name)) {
			return m_textures[name];
		}
		auto texture = Texture2D::Create(filepath);
		Add(name, texture);
		return texture;
	}

	std::shared_ptr<Texture2D> TextureLibrary::Get(const std::string& name) {
		if (Exist(name)) return m_textures[name];
		return nullptr;
	}

}