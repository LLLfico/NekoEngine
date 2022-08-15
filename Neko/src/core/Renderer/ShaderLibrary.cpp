#include "pch.h"
#include "ShaderLibrary.h"


namespace Neko {

	ShaderLibrary::ShaderLibrary() {
		Load("PBR", "assets/shaders/ShaderModel.glsl");
		// 2D
		Load("Quad", "assets/shaders/ShaderQuad.glsl");
		Load("Circle", "assets/shaders/ShaderCircle.glsl");
		Load("Line", "assets/shaders/ShaderLine.glsl");
		// IBL
		Load("Background", "assets/shaders/ShaderBackground.glsl");
		Load("HDR", "assets/shaders/ShaderHDR.glsl");
		Load("Iradiance", "assets/shaders/ShaderIradiance.glsl");
		Load("Prefliter", "assets/shaders/ShaderPrefliter.glsl");
		Load("BRDF", "assets/shaders/ShaderBRDF.glsl");
		// shadow
		Load("ShadowMap", "assets/shaders/ShaderDepth.glsl");
	}

	void ShaderLibrary::Add(const std::shared_ptr<Shader> shader) {
		auto name = shader->GetName();
		NEKO_CORE_ASSERT(!Exist(name), "Shader already exists!");
		m_shaders[name] = shader;
	}

	void ShaderLibrary::Add(const std::string& name, const std::shared_ptr<Shader> shader) {
		if (Exist(name)) {
			NEKO_CORE_WARN("{0} shader already exists!", name);
			return;
		}
		m_shaders[name] = shader;
	}

	std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& filepath) {
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath) {
		auto shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}

	std::shared_ptr<Shader> ShaderLibrary::GetShader(const std::string& name) {
		if (Exist(name)) {
			return m_shaders[name];
		}
		return nullptr;
	}

	bool ShaderLibrary::Exist(const std::string& name) const {
		return m_shaders.count(name);
	}

}