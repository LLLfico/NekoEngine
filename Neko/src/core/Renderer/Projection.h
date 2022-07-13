#pragma once

#include <glm/glm.hpp>

namespace Neko {

	class Projection {
	public:
		Projection() = default;
		Projection(const glm::mat4& projection) : m_projection(projection) {}
		virtual ~Projection() = default;

		const glm::mat4& GetProjection() const { return m_projection; }
	protected:
		glm::mat4 m_projection = glm::mat4(1.0f);
	};

}