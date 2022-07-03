#pragma once
#include "core/Renderer/Shader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Neko {

	class OpenGLShader : public Shader {
	public:
		OpenGLShader(const std::string& filePath);
		OpenGLShader(const std::string& name, const std::string& vertexDesc, const std::string& fragDesc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const std::string GetName() const { return m_name; }
		virtual void SetName(const std::string& name) { m_name = name; }

		void SetBool(const std::string& name, bool value) const override;
		void SetInt(const std::string& name, int value) const override;
		void SetFloat(const std::string& name, float value) const override;
		void SetMat4(const std::string& name, glm::mat4 value) const override;
		void SetVec3(const std::string& name, glm::vec3 value) const override;
		void SetVec2(const std::string& name, glm::vec2 value) const override;
	private:
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
	private:
		uint32_t m_id;
		std::string m_name;
	};

}