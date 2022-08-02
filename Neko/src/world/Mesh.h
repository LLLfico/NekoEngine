#pragma once
#include "core/renderer/Material.h"

#include "Animation.h"
#include "Animator.h"
#include "BoneInfo.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

#define MAX_BONE_INFLUENCE 4

namespace Neko {

	class VertexArray;
	class VertexBuffer;
	class IndexBuffer;
	class Shader;

	struct MeshVertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texcoord;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		int entityId;

		int boneIds[MAX_BONE_INFLUENCE];
		float weights[MAX_BONE_INFLUENCE];
	};

	class Mesh;
	class SubMesh {
	public:
		SubMesh() = default;
		SubMesh(const SubMesh&) = default;
		SubMesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices, int submeshIndex);

		void SetupMesh(int entityId);

		void Draw(const glm::mat4& transform, std::shared_ptr<Shader> shader, int entityId, Mesh* mesh);
	private:
		uint32_t m_submeshIndex = 0;
		std::vector<MeshVertex> m_vertices;
		std::vector<uint32_t> m_indices;
		int m_entityId = -1;

		std::shared_ptr<VertexArray> m_vao;
		std::shared_ptr<VertexBuffer> m_vbo;
		std::shared_ptr<IndexBuffer> m_ibo;
	};

	class Mesh {
	public:
		Mesh() = default;
		Mesh(const Mesh&) = default;
		Mesh(const std::string& path);

		void Draw(const glm::mat4& transform, std::shared_ptr<Shader> shader, int entityId);

		const std::string& GetPath() const { return m_path; }
		const Material& GetMaterial(uint32_t submeshIndex) const { return m_materials[submeshIndex]; }
		const std::vector<Material>& GetMaterials() const { return m_materials; }
		size_t GetSubMeshNum() const { return m_submeshes.size(); }
		const std::vector<glm::mat4> GetBoneMatrices() const { return m_animator.GetFinalBoneMatrices(); }
		int GetBoneCount() const { return m_boneCounter; }

		std::vector<Material>& GetMaterialsRef() { return m_materials; }
		std::map<std::string, BoneInfo>& GetBoneInfoMapRef() { return m_boneInfoMap; }
		int& GetBoneCountRef() { return m_boneCounter; }


	private:
		void LoadModel(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene, uint32_t& submeshIndex);
		SubMesh ProcessMesh(aiMesh* mesh, const aiScene* scene, uint32_t& submeshIndex);
		std::vector<MaterialTexture> LoadMaterialTexture(aiMaterial* material, aiTextureType type, uint32_t submeshIndex);

		void SetVertexBoneDataDefault(MeshVertex& vertex);
		void ExtractBoneWeightForVertices(std::vector<MeshVertex>& vertices, aiMesh* mesh, const aiScene* scene);

	private:
		std::vector<Material> m_materials;
		std::vector<SubMesh> m_submeshes;
		std::string m_path;
		std::string m_directory;

		Animation m_animation;
		Animator m_animator;

		bool m_animated = false;
		std::map<std::string, BoneInfo> m_boneInfoMap;
		int m_boneCounter = 0;
	};

}