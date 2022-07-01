#include "pch.h"
#include "OpenGLShader.h"

#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

namespace Neko {

	OpenGLShader::OpenGLShader(const std::string& vertexDesc, const std::string& fragDesc) {
		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = vertexDesc.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		// Compile the vertex shader
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(vertexShader);

			NEKO_CORE_ERROR("{0}", infoLog.data());
			NEKO_CORE_ASSERT(false, "Vertex shader compilation failure!");
			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = fragDesc.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		// Compile the fragment shader
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(fragmentShader);
			// Either of them. Don't leak shaders.
			glDeleteShader(vertexShader);

			NEKO_CORE_ERROR("{0}", infoLog.data());
			NEKO_CORE_ASSERT(false, "Fragment shader compilation failure!");
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		m_id = glCreateProgram();
		GLuint program = m_id;

		// Attach our shaders to our program
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);

		// Link our program
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
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			NEKO_CORE_ERROR("{0}", infoLog.data());
			NEKO_CORE_ASSERT(false, "Shader link failure!");
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);
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

}