#pragma once

#include "core/Renderer/Projection.h"

namespace Neko {

	class SceneCamera : public Projection {
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrthographic(float size, float nearClip, float farClip);
		void SetViewportSize(uint32_t width, uint32_t height);
		void SetOrthographicSize(float size);

		float GetOrthographicSize() { return m_orthographicSize; }
	private:
		void RecaculateProjection();
	private:
		float m_orthographicSize = 10.0f;
		float m_orthographicNear = -1.0f, m_orthographicFar = 1.0f;
		float m_aspectRatio = 0.0f;
	};

}