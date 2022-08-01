#include "pch.h"
#include "Animator.h"

#include "Bone.h"
#include "Mesh.h"
#include "Animation.h"

namespace Neko {

	Animator::Animator(Animation* animation) : m_currentAnimation(animation) {
		m_finalBoneMatrice.reserve(100);
		for (int i = 0; i < 100; i++) {
			m_finalBoneMatrice.push_back(glm::mat4(1.0f));
		}
	}

	void Animator::UpdateAnimation(float dt) {
		m_deltaTime = dt;
		if (m_currentAnimation) {
			m_currentTime += m_currentAnimation->GetTicksPerSecond() * dt;
			m_currentTime = fmod(m_currentTime, m_currentAnimation->GetDuration());
			CaculateBoneTransform(&m_currentAnimation->GetRootNode(), glm::mat4(1.0f));
		}
	}

	void Animator::PlayAnimation(Animation* animation) {
		m_currentAnimation = animation;
		m_currentTime = 0.0f;
	}

	void Animator::CaculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform) {
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		Bone* bone = m_currentAnimation->FindBone(nodeName);

		if (bone) {
			bone->OnUpdate(m_currentTime);
			nodeTransform = bone->GetLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = m_currentAnimation->GetBoneMap();
		if (boneInfoMap.count(nodeName)) {
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			m_finalBoneMatrice[index] = globalTransformation * offset;
		}

		for (size_t i = 0; i < node->childrenCount; i++) {
			CaculateBoneTransform(&node->children[i], globalTransformation);
		}
	}

}