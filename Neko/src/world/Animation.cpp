#include "pch.h"
#include "Animation.h"

#include "Mesh.h"
#include <assimp/Importer.hpp>

namespace Neko {

	glm::mat4 GLMMat4(const aiMatrix4x4& in) {
		glm::mat4 out;
		out[0][0] = in.a1; out[1][0] = in.a2; out[2][0] = in.a3; out[3][0] = in.a4;
		out[0][1] = in.b1; out[1][1] = in.b2; out[2][1] = in.b3; out[3][1] = in.b4;
		out[0][2] = in.c1; out[1][2] = in.c2; out[2][2] = in.c3; out[3][2] = in.c4;
		out[0][3] = in.d1; out[1][3] = in.d2; out[2][3] = in.d3; out[3][3] = in.d4;
		return out;
	}

	Animation::Animation(const std::string& animationPath, Mesh* mesh) {
		Assimp::Importer importer;

		static const uint32_t s_MeshImportFlags =
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate;
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
		const aiScene* scene = importer.ReadFile(animationPath, s_MeshImportFlags);
		NEKO_CORE_ASSERT(scene && scene->mRootNode);
		auto animation = scene->mAnimations[0];
		m_duration = animation->mDuration;
		m_ticksPerSecond = animation->mTicksPerSecond;
		ReadHeirarchyData(m_rootNode, scene->mRootNode);
		ReadMissBones(animation, *mesh);
	}

	Bone* Animation::FindBone(const std::string& name) {
		auto it = std::find_if(m_bones.begin(), m_bones.end(), [&](const Bone& bone) {return bone.GetBoneName() == name; });
		if (it == m_bones.end()) return nullptr;
		return &(*it);
	}

	void Animation::ReadMissBones(const aiAnimation* animation, Mesh& mesh) {
		int size = animation->mNumChannels;

		auto& boneInfoMap = mesh.GetBoneInfoMapRef();
		int& boneCount = mesh.GetBoneCountRef();

		for (int i = 0; i < size; i++) {
			auto channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.data;

			if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
				boneInfoMap[boneName].id = boneCount;
				boneCount++;
			}
			m_bones.emplace_back(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel);
		}
		m_boneInfoMap = boneInfoMap;
	}

	void Animation::ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src) {
		NEKO_CORE_ASSERT(src);

		dest.name = src->mName.data;
		dest.transformation = GLMMat4(src->mTransformation);
		dest.childrenCount = src->mNumChildren;

		for (int i = 0; i < src->mNumChildren; ++i) {
			AssimpNodeData child;
			ReadHeirarchyData(child, src->mChildren[i]);
			dest.children.push_back(child);
		}
	}
}