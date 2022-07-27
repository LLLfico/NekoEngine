#pragma once

#include "Projection.h"
#include "core/TimeStep.h"
#include "core/events/Event.h"
#include "core/events/MouseEvent.h"

namespace Neko {

	class EditorCamera : public Projection {
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float zNear, float zFar);

		void OnUpdate(TimeStep dt);
		void OnEvent(Event& e);

		float GetDistance() const { return m_distance; }
		const glm::mat4& GetViewMatrix() const { return m_view; }
		const glm::mat4& GetProjectionMatrix() const { return m_projection; }
		const glm::mat4 GetMatrix() const { return m_projection * m_view; }
		glm::vec3 GetUp() const;
		glm::vec3 GetRight() const;
		glm::vec3 GetForward() const;
		const glm::vec3& GetPosition() const { return m_position; }
		glm::quat GetOrientation() const;
		float GetPitch() const { return m_pitch; }
		float GetYaw() const { return m_yaw; }
		float GetNearPlane() const { return m_zNear; }
		float GetFarPlane() const { return m_zFar; }

		void SetDistance(float distance) { m_distance = distance; }
		void SetViewportSize(float width, float height) {
			m_viewportWidth = width;
			m_viewportHeight = height;
			UpdateProjection();
		}

	private:
		void UpdateProjection();
		void UpdateView();

		bool OnMouseScroll(MouseScrolledEvent& e);

		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		glm::vec3 CaculatePosition() const;

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;

	private:
		float m_fov = 45.0f;
		float m_aspectRatio = 1.778f;
		float m_zNear = 0.1f;
		float m_zFar = 1000.0f;

		glm::mat4 m_view;
		glm::vec3 m_position{ 0.0f, 0.0f, 0.0f };
		glm::vec3 m_focalPoint = { 0.0f, 0.0f, 0.0f };

		glm::vec2 m_initialMousePosition = { 0.0f, 0.0f };

		float m_distance = 10.0f;
		float m_pitch = 0.0f, m_yaw = 0.0f;

		float m_viewportWidth = 1280.0f;
		float m_viewportHeight = 720.0f;
	};


}


