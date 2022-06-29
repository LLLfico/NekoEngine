#include "pch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Neko {

	std::shared_ptr<Camera> Camera::Create(CameraInfo info) {
		switch (info.type) {
		case CameraType::Orthographic: {
			return std::make_shared<OrthographicCamera>(info.left, info.right, info.bottom, info.top);
		}
		case CameraType::Perspective: {
			return std::make_shared<PerspectiveCamera>(glm::radians(info.fov), info.aspect, info.zNear, info.zFar);
		}
		}

		NEKO_ASSERT(false, "Invalid Camera Type!");
		return nullptr;
	}

	/////////////////////////////
	//      Orthographic
	////////////////////////////
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top) : m_projection(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)) {
		m_viewProjection = m_projection * m_view;
	}

	void OrthographicCamera::SetPosition(const glm::vec3& position) {
		m_position = position;
		CaculateMatrix();
	}

	void OrthographicCamera::SetRotation(const glm::vec3& rotation) {
		m_rotation = rotation;
		CaculateMatrix();
	}

	void OrthographicCamera::CaculateMatrix() {
		auto transform = glm::translate(glm::mat4(1.0f), m_position) * glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), glm::vec3(0, 0, 1));

		m_view = glm::inverse(transform);
		m_viewProjection = m_projection * m_view;
	}



	/////////////////////////////
	//      Perspective
	////////////////////////////

	PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float zNear, float zFar) {

	}

	void PerspectiveCamera::SetPosition(const glm::vec3& position) {
		m_position = position;
		CaculateMatrix();
	}

	void PerspectiveCamera::SetRotation(const glm::vec3& rotation) {
		m_rotation = rotation;
		CaculateMatrix();
	}

	void PerspectiveCamera::CaculateMatrix() {

	}

}