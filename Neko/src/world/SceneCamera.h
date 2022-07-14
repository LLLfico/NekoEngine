#pragma once

#include "core/Renderer/Projection.h"

namespace Neko {

	class SceneCamera : public Projection {
	public:
		enum class ProjectionType {
			Perspective = 0,
			Orthographic = 1,
			Count,
		};
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrthographic(float size, float nearClip, float farClip);
		void SetPerspective(float fov, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t height);

		void SetPerspectiveFov(float fov);
		void SetPerspectiveNearClip(float nearClip);
		void SetPerspectiveFarClip(float farClip);
		void SetOrthographicSize(float size);
		void SetOrthographicNearClip(float nearClip);
		void SetOrthographicFarClip(float farClip);
		void SetProjectionType(ProjectionType type);

		float GetPerspectiveFov() { return m_perspectiveFov; }
		float GetPerspectiveNearClip() { return m_perspectiveNear; }
		float GetPerspectiveFarClip() { return m_perspectiveFar; }
		float GetOrthographicSize() { return m_orthographicSize; }
		float GetOrthographicNearClip() { return m_orthographicNear; }
		float GetOrthographicFarClip() { return m_orthographicFar; }
		ProjectionType GetProjectionType() { return m_type; }
	private:
		void RecaculateProjection();
	private:
		ProjectionType m_type = ProjectionType::Orthographic;

		float m_perspectiveFov = glm::radians(45.0f);
		float m_perspectiveNear = 0.1f, m_perspectiveFar = 1000.0f;

		float m_orthographicSize = 10.0f;
		float m_orthographicNear = -1.0f, m_orthographicFar = 1.0f;

		float m_aspectRatio = 0.0f;
	};

}