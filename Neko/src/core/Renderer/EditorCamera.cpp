#include "pch.h"
#include "EditorCamera.h"

#include "core/Input.h"
#include "core/KeyCodes.h"
#include "core/MouseCodes.h"

#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Neko {

	EditorCamera::EditorCamera(float fov, float aspectRatio, float zNear, float zFar) : m_fov(fov), m_aspectRatio(aspectRatio), m_zNear(zNear), m_zFar(zFar), Projection(glm::perspective(glm::radians(fov), aspectRatio, zNear, zFar)) {
		UpdateView();
	}

	void EditorCamera::OnUpdate(TimeStep dt) {
		if (Input::IsKeyPressed(Key::LeftAlt)) {
			const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
			glm::vec2 delta = (mouse - m_initialMousePosition) * 0.003f;
			m_initialMousePosition = mouse;
			if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
				MousePan(delta);
			else if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
				MouseRotate(delta);
			else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
				MouseZoom(delta.y);
		}

		UpdateView();
	}

	void EditorCamera::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(NEKO_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
	}

	glm::vec3 EditorCamera::GetUp() const {
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	glm::vec3 EditorCamera::GetRight() const {
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, -0.0f));
	}
	glm::vec3 EditorCamera::GetForward() const {
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}
	glm::quat EditorCamera::GetOrientation() const {
		return glm::quat(glm::vec3(-m_pitch, -m_yaw, 0.0f));
	}

	void EditorCamera::UpdateProjection() {
		m_aspectRatio = m_viewportWidth / m_viewportHeight;
		m_projection = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_zNear, m_zFar);
	}

	void EditorCamera::UpdateView() {
		m_position = CaculatePosition();
		glm::quat orientation = GetOrientation();
		m_view = glm::translate(glm::mat4(1.0f), m_position) * glm::toMat4(orientation);
		m_view = glm::inverse(m_view);
	}

	bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e) {
		float delta = e.GetYOffset() * 0.1f;
		MouseZoom(delta);
		UpdateView();
		return false;
	}

	void EditorCamera::MousePan(const glm::vec2& delta) {
		auto [xSpeed, ySpeed] = PanSpeed();
		m_focalPoint += -GetRight() * delta.x * xSpeed * m_distance;
		m_focalPoint += GetUp() * delta.y * ySpeed * m_distance;
	}

	void EditorCamera::MouseRotate(const glm::vec2& delta) {
		float yawSign = GetUp().y < 0 ? -1.0f : 1.0f;
		m_yaw += yawSign * delta.x * RotationSpeed();
		m_pitch += delta.y * RotationSpeed();
	}
	
	void EditorCamera::MouseZoom(float delta) {
		m_distance -= delta * ZoomSpeed();
		if (m_distance < 1.0f) {
			m_focalPoint += GetForward();
			m_distance = 1.0f;
		}
	}

	glm::vec3 EditorCamera::CaculatePosition() const {
		return m_focalPoint - GetForward() * m_distance;
	}

	std::pair<float, float> EditorCamera::PanSpeed() const {
		float x = std::min(m_viewportWidth / 1000.0f, 2.4f);
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_viewportHeight / 1000.0f, 2.4f);
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float EditorCamera::RotationSpeed() const {
		return 0.8f;
	}
	float EditorCamera::ZoomSpeed() const {
		float distance = m_distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f);
		return speed;
	}
}

