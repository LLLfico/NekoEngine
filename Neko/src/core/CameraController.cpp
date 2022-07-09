#include "pch.h"
#include "CameraController.h"

#include "TimeStep.h"
#include "Input.h"
#include "KeyCodes.h"
#include "Renderer/Camera.h"
#include "events/MouseEvent.h"
#include "events/ApplicationEvent.h"


namespace Neko {

	void CameraController::OnUpdate(TimeStep dt) {

		auto right = m_camera.GetRight();
		auto front = m_camera.GetForward();

		auto position = m_camera.GetPosition();
		if (Neko::Input::IsKeyPressed(NEKO_KEY_A))
			position -= m_translationSpeed * dt * right;
		else if (Neko::Input::IsKeyPressed(NEKO_KEY_D))
			position += m_translationSpeed * dt * right;

		if (Neko::Input::IsKeyPressed(NEKO_KEY_W))
			position += m_translationSpeed * dt * front;
		else if (Neko::Input::IsKeyPressed(NEKO_KEY_S))
			position -= m_translationSpeed * dt * front;
		m_camera.SetPosition(position);
	}

	void CameraController::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);
		// NEKO_CORE_TRACE("{0}", e);
		dispatcher.Dispatch<MouseScrolledEvent>(NEKO_BIND_EVENT_FN(CameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(NEKO_BIND_EVENT_FN(CameraController::OnWindowResized));
		if (m_camera.GetType() == CameraType::Perspective) {
			dispatcher.Dispatch<MouseMovedEvent>(NEKO_BIND_EVENT_FN(CameraController::OnMouseMoved));
		}
	}

	bool CameraController::OnMouseScrolled(MouseScrolledEvent& e) {
		float yoffset = e.GetYOffset();
		float fov = m_camera.GetFov();
		fov -= yoffset * m_scrollSensitivity;
		m_camera.SetFov(fov);
		return false;
	}

	bool CameraController::OnWindowResized(WindowResizeEvent& e) {
		m_aspect = (float)e.GetWidth() / (float)e.GetHeight();
		m_camera.SetAspectRatio(m_aspect);
		return false;
	}

	bool CameraController::OnMouseMoved(MouseMovedEvent& e) {
		if (m_firstMouseMove) {
			lastScreenX = e.GetX();
			lastScreenY = e.GetY();
			m_firstMouseMove = false;
			return false;
		}

		auto curX = e.GetX();
		auto curY = e.GetY();
		auto xoffset = curX - lastScreenX;
		auto yoffset = curY - lastScreenY;
		lastScreenX = curX;
		lastScreenY = curY;

		float yaw = m_camera.GetYaw();
		yaw += xoffset * m_moveSensitivity;

		float pitch = m_camera.GetPitch();
		pitch -= yoffset * m_moveSensitivity;
		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;

		m_camera.SetRotation(yaw, pitch);
		return false;
	}
}

