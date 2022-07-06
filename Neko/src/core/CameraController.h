#pragma once
#include "core/Core.h"

#include "Renderer/Camera.h"
#include <glm/glm.hpp>

namespace Neko {

	class OrthographicCamera;
	class Time;
	class Event;
	class MouseScrolledEvent;
	class WindowResizeEvent;

	class NEKO_API OrthographicCameraController {
	public:
		OrthographicCameraController(float aspect, bool rotationOn = false);

		void OnUpdate(Time dt);
		void OnEvent(Event& e);

		const OrthographicCamera& GetCamera() const { return m_camera; }
		float GetZoomLevel() const { return m_zoom; }
		void SetZoomLevel(float zoom) { m_zoom = zoom; }
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_aspect = (float)1280 / (float)720;
		float m_zoom = 1.0f;

		OrthographicCamera m_camera;

		bool m_rotationOn;

		glm::vec3 m_position = { 0.0f, 0.0f, 0.0f };
		float m_rotation = 0.0f;

		float m_translationSpeed = 1.0f;
		float m_rotationSpeed = 180.0f;
	};

}