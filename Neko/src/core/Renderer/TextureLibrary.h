#pragma once
#include "Texture.h"

namespace Neko {

	class TextureLibrary {
	public:
		TextureLibrary();

		void Add(const std::string& name, const std::shared_ptr<Texture2D> texture);
		std::shared_ptr<Texture2D> Load(const std::string& filepath);
		std::shared_ptr<Texture2D> Load(const std::string& name, const std::string& filepath);

		bool Exist(const std::string& name) { return m_textures.count(name); }

		std::shared_ptr<Texture2D> Get(const std::string& name);

		static TextureLibrary GetInstance() {
			static TextureLibrary s_instance;
			return s_instance;
		}
	private:
		std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_textures;
	};

}