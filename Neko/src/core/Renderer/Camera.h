#pragma once
#include "core/Core.h"

#include <glm/glm.hpp>

namespace Neko {

	enum class CameraType {
		Orthographic = 0,
		Perspective,
	};

	class NEKO_API Camera {
	public:
		Camera();

		void SetOrthographic(float aspect);
		void SetPerspective(float fov, float aspect, float zNear, float zFar);

		CameraType GetType() { return m_type; }

		const glm::vec3& GetPosition() const { return m_position; }

		const glm::mat4& GetViewMatrix() const { return m_view; }
		const glm::mat4& GetProjectionMatrix() const { return m_projection; }
		const glm::mat4& GetMatrix() const { return m_viewProjection; }

		const glm::vec3& GetForward() { return m_forward; }
		const glm::vec3& GetRight() { return m_right; }
		float GetFov() { return m_fov; }
		float GetYaw() { return m_yaw; }
		float GetPitch() { return m_pitch; }

		void SetPosition(const glm::vec3& position);
		void SetYaw(float yaw);
		void SetPitch(float pitch);
		void SetRotation(float yaw, float pitch);
		void SetFov(float fov);
		void SetAspectRatio(float aspect);
	private:
		void CaculateCameraTowards();
		void CaculateMatrix();
	private:
		CameraType m_type = CameraType::Perspective;
		float m_fov = 45.0f;
		float m_aspect = (float)16 / (float)9;
		float m_zNear = 0.1f, m_zFar = 1000.0f;

		glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 3.0f);

		float m_yaw = -90.0f;
		float m_pitch = 0.0f;
		float m_roll = 0.0f;

		glm::vec3 m_forward = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 m_right = glm::vec3(-1.0f, 0.0f, 0.0f);

		glm::mat4 m_projection = glm::mat4(1.0f);
		glm::mat4 m_view = glm::mat4(1.0f);
		glm::mat4 m_viewProjection = glm::mat4(1.0f);
	};
}