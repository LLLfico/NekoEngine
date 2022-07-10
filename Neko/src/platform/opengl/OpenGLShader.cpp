#include "pch.h"
#include "OpenGLShader.h"

#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

namespace Neko {

	// local static 
	std::string ReadFile(const std::string& filePath) {
		std::string result;
		std::ifstream in(filePath, std::ios::in | std::ios::binary);
		if (in) {
			in.seekg(0, std::ios::end); // get file string size
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(result.data(), result.size());
			in.close();
		}
		else {
			NEKO_CORE_ERROR("Cannot Open File {0}!", filePath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> PreProcess(const std::string& source) {
		static std::unordered_map<std::string, GLenum> shaderTypeFromString = {
			{ "vertex", GL_VERTEX_SHADER },
			{ "fragment", GL_FRAGMENT_SHADER },
			{ "pixel", GL_FRAGMENT_SHADER },
		};

		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t tokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);

		while (pos != std::string::npos) {
			// std::cout << source << std::endl;
			// get type substr
			size_t eol = source.find_first_of("\n\r", pos);
			NEKO_CORE_ASSERT(eol != std::string::npos, "Syntax error!");
			size_t begin = pos + tokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			NEKO_CORE_ASSERT(shaderTypeFromString.count(type), "Unknoen shader type!");

			size_t textStart = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, textStart);

			size_t textLen = textStart == std::string::npos ? pos - (source.size() - 1) : pos - textStart;
			shaderSources[shaderTypeFromString[type]] = source.substr(textStart, textLen);
		}

		return shaderSources;
	}

	// member 
	OpenGLShader::OpenGLShader(const std::string& filePath) {
		auto source = ReadFile(filePath);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);

		auto lastSlash = filePath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filePath.rfind('.');
		lastDot = lastDot == std::string::npos ? filePath.size() : lastDot;
		auto len = lastDot - lastSlash;
		m_name = filePath.substr(lastSlash, len);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexDesc, const std::string& fragDesc) : m_name(name) {

		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexDesc;
		sources[GL_FRAGMENT_SHADER] = fragDesc;
		Compile(sources);
	}

	OpenGLShader::~OpenGLShader() {
		glDeleteProgram(m_id);
	}

	void OpenGLShader::Bind() const {
		glUseProgram(m_id);
	}

	void OpenGLShader::Unbind() const {
		glUseProgram(m_id);
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources) {

		static std::unordered_map<GLenum, std::string> shaderTypeToString = {
			{ GL_VERTEX_SHADER, "Vertex" },
			{ GL_FRAGMENT_SHADER , "Fragment" },
		};

		GLuint program = glCreateProgram();
		std::vector<GLenum> glShaderIds(shaderSources.size());

		for (auto& [type, source] : shaderSources) {
			GLuint shader = glCreateShader(type);

			const GLchar* sourceCstr = source.c_str();
			glShaderSource(shader, 1, &sourceCstr, 0);
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE) {
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shader);

				NEKO_CORE_ERROR("{0}", infoLog.data());
				NEKO_CORE_ASSERT(false, "{0} shader compilation failure!", shaderTypeToString[type]);
				break;
			}
			glAttachShader(program, shader);
			glShaderIds.push_back(shader);
		}

		m_id = program;
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE) {
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.
			for (auto id : glShaderIds) {
				glDeleteShader(id);
			}

			NEKO_CORE_ERROR("{0}", infoLog.data());
			NEKO_CORE_ASSERT(false, "Shader link failure!");
			return;
		}
		for (auto id : glShaderIds) {
			glDeleteShader(id);
		}
	}

	void OpenGLShader::SetBool(const std::string& name, bool value) const {
		glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
	}
	void OpenGLShader::SetInt(const std::string& name, int value) const {
		glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
	}
	void OpenGLShader::SetFloat(const std::string& name, float value) const {
		glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
	}
	void OpenGLShader::SetMat4(const std::string& name, glm::mat4 value) const {
		glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}
	void OpenGLShader::SetVec3(const std::string& name, glm::vec3 value) const {
		glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(value));
	}
	void OpenGLShader::SetVec2(const std::string& name, glm::vec2 value) const {
		glUniform2fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(value));
	}
	void OpenGLShader::SetVec4(const std::string& name, glm::vec4 value) const {
		glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(value));
	}
	void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count) {
		glUniform1iv(glGetUniformLocation(m_id, name.c_str()), count, values);
	}
}