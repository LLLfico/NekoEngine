#pragma once

#include "core/Core.h"

#include <glm/glm.hpp>

namespace Neko {

	class NEKO_API Shader {
	public:
		Shader(const std::string& vertexDesc, const std::string& fragDesc);
		~Shader();

		void Bind() const;
		void Unbind() const;

	public:
		void SetBool(const std::string& name, bool value) const;
		void SetInt(const std::string& name, int value) const;
		void SetFloat(const std::string& name, float value) const;
		void SetMat4(const std::string& name, glm::mat4 value) const;
		void SetVec3(const std::string& name, glm::vec3 value) const;
		void SetVec2(const std::string& name, glm::vec2 value) const;
	private:
		unsigned int m_id;
	};

}

