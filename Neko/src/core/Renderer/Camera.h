#pragma once
#include "core/Core.h"

#include <glm/glm.hpp>

namespace Neko {

	enum class CameraType {
		Orthographic = 0,
		Perspective,
	};

	class NEKO_API OrthographicCamera {
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		const glm::vec3& GetPosition() const { return m_position; }
		float GetRotation() const { return m_rotation; }

		const glm::mat4& GetViewMatrix() const { return m_view; }
		const glm::mat4& GetProjectionMatrix() const { return m_projection; }
		const glm::mat4& GetMatrix() const { return m_viewProjection; }

		void SetProjection(float left, float right, float bottom, float top);
		void SetPosition(const glm::vec3& position);
		void SetRotation(float rotation);

	private:
		void CaculateMatrix();
	private:
		CameraType type = CameraType::Orthographic;

		glm::mat4 m_projection = glm::mat4(1.0f);
		glm::mat4 m_view = glm::mat4(1.0f);
		glm::mat4 m_viewProjection = glm::mat4(1.0f);

		glm::vec3 m_position = glm::vec3(0.0f);
		float m_rotation = 0.0f;
	};

}