#include "pch.h"
#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Neko{

	Neko::SceneCamera::SceneCamera() {
		RecaculateProjection();
	}

	void Neko::SceneCamera::SetOrthographic(float size, float nearClip, float farClip) {
		m_orthographicSize = size;
		m_orthographicNear = nearClip;
		m_orthographicFar = farClip;
		RecaculateProjection();
	}

	void Neko::SceneCamera::SetViewportSize(uint32_t width, uint32_t height) {
		m_aspectRatio = (float)width / (float)height;
		RecaculateProjection();
	}

	void Neko::SceneCamera::SetOrthographicSize(float size) {
		m_orthographicSize = size;
		RecaculateProjection();
	}

	void Neko::SceneCamera::RecaculateProjection() {
		float left = -m_orthographicSize * m_aspectRatio * 0.5f;
		float right = m_orthographicSize * m_aspectRatio * 0.5f;
		float bottom = -m_orthographicSize * 0.5f;
		float top = m_orthographicSize * 0.5f;

		m_projection = glm::ortho(left, right, bottom, top, m_orthographicNear, m_orthographicFar);
	}

}
