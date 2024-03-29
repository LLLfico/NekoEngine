#pragma once

#include "core/Core.h"

#include <glm/glm.hpp>

namespace Neko {

	class NEKO_API Shader {
	public:
		~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual const std::string GetName() const = 0;
		virtual void SetName(const std::string& name) = 0;

		virtual void SetBool(const std::string& name, bool value) const = 0;
		virtual void SetInt(const std::string& name, int value) const = 0;
		virtual void SetFloat(const std::string& name, float value) const = 0;
		virtual void SetMat4(const std::string& name, glm::mat4 value) const = 0;
		virtual void SetVec3(const std::string& name, glm::vec3 value) const = 0;
		virtual void SetVec2(const std::string& name, glm::vec2 value) const = 0;
		virtual void SetVec4(const std::string& name, glm::vec4 value) const = 0;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;

		static std::shared_ptr<Shader> Create(const std::string& filePath);
		static std::shared_ptr<Shader> Create(const std::string& name, const std::string& vertexDesc, const std::string& fragDesc);
	};

}

