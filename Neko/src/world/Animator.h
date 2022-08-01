#pragma once


#include <glm/glm.hpp>

namespace Neko {

	class Animation;
	struct AssimpNodeData;

	class Animator {
	public:
		Animator() = default;
		Animator(const Animator&) = default;
		Animator(Animation* animation);

		void UpdateAnimation(float dt);
		void PlayAnimation(Animation* animation);
		void CaculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
		const std::vector<glm::mat4>& GetFinalBoneMatrices() { return m_finalBoneMatrice; }
	private:
		std::vector<glm::mat4> m_finalBoneMatrice;
		Animation* m_currentAnimation = nullptr;;
		float m_currentTime = 0.0f;
		float m_deltaTime = 0.0f;
	};

}