#include "pch.h"
#include "Bone.h"

namespace Neko {

	glm::vec3 GLMVec3(const aiVector3D& in) {
		return glm::vec3(in.x, in.y, in.z);
	}
	glm::quat GLMQuat(const aiQuaternion& in) {
		return glm::quat(in.w, in.x, in.y, in.z);
	}

	Bone::Bone(const std::string& name, int id, const aiNodeAnim* channel) : m_name(name), m_id(id), m_localTransform(1.0f) {
		m_numPositions = channel->mNumPositionKeys;
		for (size_t positionIndex = 0; positionIndex < m_numPositions; ++positionIndex) {
			aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
			float timeStep = channel->mPositionKeys[positionIndex].mTime;
			// KeyPosition data{ GLMVec(aiPosition), timeStep };
			m_positions.push_back({ GLMVec3(aiPosition), timeStep });
		}

		m_numRotations = channel->mNumRotationKeys;
		for (size_t rotationIndex = 0; rotationIndex < m_numRotations; ++rotationIndex) {
			aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
			float timeStep = channel->mRotationKeys[rotationIndex].mTime;
			m_rotations.push_back({ GLMQuat(aiOrientation), timeStep });
		}

		m_numScales = channel->mNumScalingKeys;
		for (size_t scaleIndex = 0; scaleIndex < m_numScales; ++scaleIndex) {
			aiVector3D aiScale = channel->mScalingKeys[scaleIndex].mValue;
			float timeStep = channel->mScalingKeys[scaleIndex].mTime;
			m_scales.push_back({ GLMVec3(aiScale), timeStep });
		}
	}

	void Bone::OnUpdate(float animationTime) {
		auto translation = InterpolatePosition(animationTime);
		auto rotation = InterpolateRotation(animationTime);
		auto scale = InterpolateScaling(animationTime);
		m_localTransform = translation * rotation * scale;
	}

	int Bone::GetPositionIndex(float animationTime) {
		for (size_t i = 0; i < m_numPositions - 1; ++i) {
			if (animationTime < m_positions[i + 1].timeStep)
				return i;
		}
		NEKO_CORE_ASSERT(false);
	}

	int Bone::GetRotationIndex(float animationTime) {
		for (size_t i = 0; i < m_numRotations - 1; ++i) {
			if (animationTime < m_rotations[i + 1].timeStep)
				return i;
		}
		NEKO_CORE_ASSERT(false);
	}

	int Bone::GetScaleIndex(float animationTime) {
		for (size_t i = 0; i < m_numScales - 1; ++i) {
			if (animationTime < m_scales[i + 1].timeStep)
				return i;
		}
		NEKO_CORE_ASSERT(false);
	}

	float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
		float scaleFactor = 0.0f;
		float midWayLength = animationTime - lastTimeStamp;
		float frameDiff = nextTimeStamp - lastTimeStamp;
		scaleFactor = midWayLength / frameDiff;
		return scaleFactor;
	}

	glm::mat4 Bone::InterpolatePosition(float animationTime) {
		if (m_numPositions == 1) {
			return glm::translate(glm::mat4(1.0f), m_positions[0].position);
		}
		int p0 = GetPositionIndex(animationTime);
		int p1= p0 + 1;
		float scaleFactor = GetScaleFactor(m_positions[p0].timeStep, m_positions[p1].timeStep, animationTime);
		glm::vec3 finalPosition = glm::mix(m_positions[p0].position, m_positions[p1].position, scaleFactor);
		return glm::translate(glm::mat4(1.0f), finalPosition);
	}

	glm::mat4 Bone::InterpolateRotation(float animationTime) {
		if (m_numRotations == 1) {
			auto rotation = glm::normalize(m_rotations[0].orientation);
			return glm::toMat4(rotation);
		}
		int p0 = GetRotationIndex(animationTime);
		int p1 = p0 + 1;
		float scaleFactor = GetScaleFactor(m_rotations[p0].timeStep, m_rotations[p1].timeStep, animationTime);
		auto finalRotation = glm::slerp(m_rotations[p0].orientation, m_rotations[p1].orientation, animationTime);
		return glm::toMat4(finalRotation);
	}

	glm::mat4 Bone::InterpolateScaling(float animationTime) {
		if (m_numScales) {
			return glm::scale(glm::mat4(1.0f), m_scales[0].scale);
		}
		int p0 = GetScaleIndex(animationTime);
		int p1 = p0 + 1;
		float scaleFactor = GetScaleFactor(m_scales[p0].timeStep, m_scales[p1].timeStep, animationTime);
		auto finalScale = glm::mix(m_scales[p0].scale, m_scales[p1].scale, scaleFactor);
		return glm::scale(glm::mat4(1.0f), finalScale);
	}

}