#pragma once

#include "Shader.h"

namespace Neko {

	class ShaderLibrary {
	public:
		ShaderLibrary();

		void Add(const std::shared_ptr<Shader> shader);
		void Add(const std::string& name, const std::shared_ptr<Shader> shader);
		std::shared_ptr<Shader> Load(const std::string& filepath);
		std::shared_ptr<Shader> Load(const std::string& name, const std::string& filepath);

		std::shared_ptr<Shader> GetShader(const std::string& name);

		bool Exist(const std::string& name) const;

		static ShaderLibrary GetInstance() {
			static ShaderLibrary s_instance;
			return s_instance;
		}

	private:
		std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
	};

}