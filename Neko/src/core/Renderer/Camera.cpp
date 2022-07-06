#include "pch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Neko {
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top) : m_projection(glm::ortho(left, right, bottom, top)) {
		m_viewProjection = m_projection * m_view;
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top) {
		m_projection = glm::ortho(left, right, bottom, top);
		m_viewProjection = m_projection * m_view;
	}

	void OrthographicCamera::SetPosition(const glm::vec3& position) {
		m_position = position;
		CaculateMatrix();
	}
	void OrthographicCamera::SetRotation(float rotation) {
		m_rotation = rotation;
		CaculateMatrix();
	}
	void OrthographicCamera::CaculateMatrix() {
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position) * glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation), glm::vec3(0, 0, 1));
		m_view = glm::inverse(transform);
		m_viewProjection = m_projection * m_view;
	}
}