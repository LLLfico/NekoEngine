#include "pch.h"
#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Neko{

	Neko::SceneCamera::SceneCamera() {
		RecaculateProjection();
	}

	void Neko::SceneCamera::SetOrthographic(float size, float nearClip, float farClip) {
		m_type = ProjectionType::Orthographic;
		m_orthographicSize = size;
		m_orthographicNear = nearClip;
		m_orthographicFar = farClip;
		RecaculateProjection();
	}

	void SceneCamera::SetPerspective(float fov, float nearClip, float farClip) {
		m_type = ProjectionType::Perspective;
		m_perspectiveFov = fov;
		m_perspectiveNear = nearClip;
		m_perspectiveFar = farClip;
		RecaculateProjection();
	}

	void Neko::SceneCamera::SetViewportSize(uint32_t width, uint32_t height) {
		m_aspectRatio = (float)width / (float)height;
		RecaculateProjection();
	}

	void SceneCamera::SetPerspectiveFov(float fov) {
		m_perspectiveFov = fov;
		RecaculateProjection();
	}

	void SceneCamera::SetPerspectiveNearClip(float nearClip) {
		m_perspectiveNear = nearClip;
		RecaculateProjection();
	}

	void SceneCamera::SetPerspectiveFarClip(float farClip) {
		m_perspectiveFar = farClip;
		RecaculateProjection();
	}

	void Neko::SceneCamera::SetOrthographicSize(float size) {
		m_orthographicSize = size;
		RecaculateProjection();
	}

	void SceneCamera::SetOrthographicNearClip(float nearClip) {
		m_orthographicNear = nearClip;
		RecaculateProjection();
	}

	void SceneCamera::SetOrthographicFarClip(float farClip) {
		m_orthographicFar = farClip;
		RecaculateProjection();
	}

	void SceneCamera::SetProjectionType(ProjectionType type) {
		m_type = type;
		RecaculateProjection();
	}

	void Neko::SceneCamera::RecaculateProjection() {
		if (m_type == ProjectionType::Orthographic) {
			float left = -m_orthographicSize * m_aspectRatio * 0.5f;
			float right = m_orthographicSize * m_aspectRatio * 0.5f;
			float bottom = -m_orthographicSize * 0.5f;
			float top = m_orthographicSize * 0.5f;

			m_projection = glm::ortho(left, right, bottom, top, m_orthographicNear, m_orthographicFar);
		}
		else {
			m_projection = glm::perspective(m_perspectiveFov, m_aspectRatio, m_perspectiveNear, m_perspectiveFar);
		}

	}

}
