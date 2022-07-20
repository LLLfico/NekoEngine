#pragma once
#include "core/Core.h"

#include "renderer/Camera.h"
#include <glm/glm.hpp>

namespace Neko {

	class OrthographicCamera;
	class TimeStep;
	class Event;
	class MouseScrolledEvent;
	class WindowResizeEvent;
	class MouseMovedEvent;

	class NEKO_API CameraController {
	public:
		CameraController() {} // m_camera.SetOrthographic(m_aspect); }

		void OnUpdate(TimeStep dt);
		void OnEvent(Event& e);

		void OnResize(uint32_t width, uint32_t height);

		const Camera& GetCamera() const { return m_camera; }

		void SetOrthographicCamera() { m_camera.SetOrthographic(m_aspect); }
		void SetPerspectiveCamera()  { m_camera.SetPerspective(45.0f, m_aspect, 0.1f, 1000.0f); }

	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
		bool OnMouseMoved(MouseMovedEvent& e);
	private:
		float m_aspect = (float)1280 / (float)720;

		Camera m_camera;

		float lastScreenX = 0.0f, lastScreenY = 0.0f;
		bool m_firstMouseMove = true;
		float m_moveSensitivity = 0.2f;
		float m_scrollSensitivity = 0.1f;

		float m_translationSpeed = 3.0f;
	};

}