#pragma once

#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Neko {
	struct KeyPosition {
		glm::vec3 position;
		float timeStep;
	};

	struct KeyRotation {
		glm::quat orientation;
		float timeStep;
	};

	struct KeyScale {
		glm::vec3 scale;
		float timeStep;
	};

	class Bone {
	public:
		Bone(const std::string& name, int id, const aiNodeAnim* channel);

		void OnUpdate(float animationTime);

		const glm::mat4& GetLocalTransform() const { return m_localTransform; }
		const std::string& GetBoneName() const { return m_name; }
		int GetBoneId() const { return m_id; }

		int GetPositionIndex(float animationTime);
		int GetRotationIndex(float animationTime);
		int GetScaleIndex(float animationTime);
	private:
		float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
		glm::mat4 InterpolatePosition(float animationTime);
		glm::mat4 InterpolateRotation(float animationTime);
		glm::mat4 InterpolateScaling(float animationTime);
	private:
		std::vector<KeyPosition> m_positions;
		std::vector<KeyRotation> m_rotations;
		std::vector<KeyScale> m_scales;
		int m_numPositions;
		int m_numRotations;
		int m_numScales;

		glm::mat4 m_localTransform;
		std::string m_name;
		int m_id;
	};
}