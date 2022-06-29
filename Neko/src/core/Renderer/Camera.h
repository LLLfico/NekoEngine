#pragma once
#include "core/Core.h"

#include <glm/glm.hpp>

namespace Neko {

	enum class CameraType {
		Orthographic = 0,
		Perspective,
	};

	struct CameraInfo {
		
		float left = -1.6f;
		float right = 1.6f;
		float bottom = -0.9f;
		float top = 0.9f;

		CameraType type = CameraType::Orthographic;

		float aspect = 16.0f / 9.0f;
		float fov = 45.0f;
		float zNear = 0.1f;
		float zFar = 1000.0f;
	};

	

	class NEKO_API Camera {
	public:
		virtual const glm::vec3& GetPosition() const = 0;
		virtual const glm::vec3& GetRotation() const = 0;

		virtual const glm::mat4& GetViewMatrix() const = 0;
		virtual const glm::mat4& GetProjectionMatrix() const = 0;
		virtual const glm::mat4& GetMatrix() const = 0;

		virtual void SetPosition(const glm::vec3& position) = 0;
		virtual void SetRotation(const glm::vec3& rotation) = 0;

		static std::shared_ptr<Camera> Create(CameraInfo info);
	protected:
		virtual void CaculateMatrix() = 0;
	};

	class NEKO_API OrthographicCamera : public Camera {
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		virtual const glm::vec3& GetPosition() const override { return m_position; };
		virtual const glm::vec3& GetRotation() const override { return m_rotation; }

		virtual const glm::mat4& GetViewMatrix() const override { return m_view; }
		virtual const glm::mat4& GetProjectionMatrix() const override { return m_projection; }
		virtual const glm::mat4& GetMatrix() const override { return m_viewProjection; }

		virtual void SetPosition(const glm::vec3& position) override;
		virtual void SetRotation(const glm::vec3& rotation) override;

	private:
		virtual void CaculateMatrix() override;
	private:
		glm::mat4 m_projection = glm::mat4(1.0f);
		glm::mat4 m_view = glm::mat4(1.0f);
		glm::mat4 m_viewProjection = glm::mat4(1.0f);

		glm::vec3 m_position = glm::vec3(0.0f);
		glm::vec3 m_rotation = glm::vec3(0.0f);
	};

	class NEKO_API PerspectiveCamera : public Camera {
	public:
		PerspectiveCamera(float fov, float aspect, float zNear, float zFar);

		virtual const glm::vec3& GetPosition() const override { return m_position; };
		virtual const glm::vec3& GetRotation() const override { return m_rotation; }

		virtual const glm::mat4& GetViewMatrix() const override { return m_view; }
		virtual const glm::mat4& GetProjectionMatrix() const override { return m_projection; }
		virtual const glm::mat4& GetMatrix() const override { return m_viewProjection; }

		virtual void SetPosition(const glm::vec3& position) override;
		virtual void SetRotation(const glm::vec3& rotation) override;
	private:
		virtual void CaculateMatrix() override;
	private:
		glm::mat4 m_projection = glm::mat4(1.0f);
		glm::mat4 m_view = glm::mat4(1.0f);
		glm::mat4 m_viewProjection = glm::mat4(1.0f);

		glm::vec3 m_position = glm::vec3(0.0f);
		glm::vec3 m_rotation = glm::vec3(0.0f);
	};

}