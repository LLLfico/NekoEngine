#pragma once

#include "Bone.h"

#include <assimp/scene.h>
#include <glm/glm.hpp>


namespace Neko {

	struct AssimpNodeData {
		glm::mat4 transformation;
		std::string name;
		int childrenCount;
		std::vector<AssimpNodeData> children;
	};

	class Mesh;
	struct BoneInfo;

	class Animation {
	public:
		Animation() = default;

		Animation(const std::string& animationPath, Mesh* model);
		~Animation() = default;

		Bone* FindBone(const std::string& name);

		inline float GetTicksPerSecond() const { return m_ticksPerSecond; }
		inline float GetDuration() const { return m_duration; }
		inline const AssimpNodeData& GetRootNode() const { return m_rootNode; }
		inline const std::map<std::string, BoneInfo>& GetBoneMap() const { return m_boneInfoMap; }
	private:
		void ReadMissBones(const aiAnimation* animation, Mesh& mesh);
		void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);
	private:
		float m_duration = 0.0f;
		int m_ticksPerSecond = 0;
		std::vector<Bone> m_bones;
		AssimpNodeData m_rootNode;
		std::map<std::string, BoneInfo> m_boneInfoMap;
	};

}