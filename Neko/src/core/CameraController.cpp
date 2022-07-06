#include "pch.h"
#include "CameraController.h"

#include "Time.h"
#include "Input.h"
#include "KeyCodes.h"
#include "Renderer/Camera.h"
#include "events/MouseEvent.h"
#include "events/ApplicationEvent.h"


namespace Neko {

	OrthographicCameraController::OrthographicCameraController(float aspect, bool rotationOn) : m_aspect(aspect), m_camera(-aspect * m_zoom, aspect * m_zoom, -m_zoom, m_zoom) {

	}

	void OrthographicCameraController::OnUpdate(Time dt) {

		if (Neko::Input::IsKeyPressed(NEKO_KEY_LEFT))
			m_position.x -= m_translationSpeed * dt;
		else if (Neko::Input::IsKeyPressed(NEKO_KEY_RIGHT))
			m_position.x += m_translationSpeed * dt;

		if (Neko::Input::IsKeyPressed(NEKO_KEY_UP))
			m_position.y += m_translationSpeed * dt;
		else if (Neko::Input::IsKeyPressed(NEKO_KEY_DOWN))
			m_position.y -= m_translationSpeed * dt;
		m_camera.SetPosition(m_position);

		if (m_rotationOn) {
			if (Neko::Input::IsKeyPressed(NEKO_KEY_A))
				m_rotation += m_rotationSpeed * dt;
			else if (Neko::Input::IsKeyPressed(NEKO_KEY_D))
				m_rotation -= m_rotationSpeed * dt;

			m_camera.SetRotation(m_rotation);
		}

		m_translationSpeed = m_zoom * 1.0f;
	}

	void OrthographicCameraController::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(NEKO_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(NEKO_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e) {
		m_zoom -= e.GetYOffset() * 0.25f;
		m_zoom = std::max(m_zoom, 0.25f);
		m_camera.SetProjection(-m_aspect * m_zoom, m_aspect * m_zoom, -m_zoom, m_zoom);
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e) {
		m_aspect = (float)e.GetWidth() / (float)e.GetHeight();
		m_camera.SetProjection(-m_aspect * m_zoom, m_aspect * m_zoom, -m_zoom, m_zoom);
		return false;
	}
}

