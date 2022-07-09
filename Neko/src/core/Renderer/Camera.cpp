#include "pch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Neko {

	Camera::Camera() {
		CaculateMatrix();
	}

	void Camera::SetOrthographic(float aspect) {
		m_type = CameraType::Orthographic;
		m_aspect = aspect;
		m_zNear = -1.0f;
		m_zFar = 1.0f;
		m_position = glm::vec3(0.0f);
		CaculateMatrix();
	}

	void Camera::SetPerspective(float fov, float aspect, float zNear, float zFar) {
		m_fov = fov;
		m_aspect = aspect;
		m_zNear = zNear;
		m_zFar = zFar;
		CaculateMatrix();
	}

	void Camera::SetPosition(const glm::vec3& position) {
		m_position = position;
		CaculateMatrix();
	}

	void Camera::SetYaw(float yaw) {
		m_yaw = yaw;
		CaculateCameraTowards();
	}

	void Camera::SetPitch(float pitch) {
		m_pitch = pitch;
		CaculateCameraTowards();
	}

	void Camera::SetRotation(float yaw, float pitch) {
		m_yaw = yaw;
		m_pitch = pitch;
		CaculateCameraTowards();
	}

	void Camera::SetFov(float fov) {
		m_fov = fov;
		CaculateMatrix();
	}

	void Camera::SetAspectRatio(float aspect) {
		m_aspect = aspect;
		CaculateMatrix();
	}

	void Neko::Camera::CaculateCameraTowards() {
		glm::vec3 front;
		front.x = cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw));
		front.y = sin(glm::radians(m_pitch));
		front.z = cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));
		m_forward = glm::normalize(front);

		static const glm::vec3 worldUp = { 0.0f, 1.0f, 0.0f };
		m_right = glm::normalize(glm::cross(m_forward, worldUp));
		m_up = glm::normalize(glm::cross(m_right, m_forward));
		CaculateMatrix();
	}

	void Camera::CaculateMatrix() {

		if (m_type == CameraType::Orthographic) {
			m_view = glm::inverse(glm::translate(glm::mat4(1.0f), m_position));
			float left = -m_aspect;
			float right = -left;
			float bottom = -m_aspect;
			float top = -bottom;
			m_projection = glm::ortho(left, right, bottom, top, m_zNear, m_zFar);
		}
		else {
			m_view = glm::lookAt(m_position, m_position + m_forward, m_up);
			m_projection = glm::perspective(m_fov, m_aspect, m_zNear, m_zFar);
		}
		m_viewProjection = m_projection * m_view;
	}
}