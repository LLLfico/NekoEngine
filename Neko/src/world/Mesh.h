#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

namespace Neko {

	class VertexArray;
	class VertexBuffer;
	class IndexBuffer;
	class Shader;

	struct MeshVertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texcoord;
		glm::vec4 color;
		int entityId;
	};

	class SubMesh {
	public:
		SubMesh() = default;
		SubMesh(const SubMesh&) = default;
		SubMesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices);

		void SetupMesh(int entityId);

		void Draw(const glm::mat4& transform, std::shared_ptr<Shader> shader, int entityId);
	private:
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

	private:
		void LoadModel(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene, uint32_t& submeshIndex);
		SubMesh ProcessMesh(aiMesh* mesh, const aiScene* scene, uint32_t& submeshIndex);

	private:
		std::vector<SubMesh> m_submeshes;
		std::string m_path;
		std::string m_directory;
	};

}