#include "pch.h"
#include "Mesh.h"

#include "core/renderer/VertexArray.h"
#include "core/renderer/Buffer.h"
#include "core/renderer/Shader.h"
#include "core/renderer/RenderCommand.h"

namespace Neko {

	Mesh::Mesh(const std::string& path) : m_path(path) {
		LoadModel(path);
	}

	void Mesh::Draw(const glm::mat4& transform, std::shared_ptr<Shader> shader, int entityId) {
		for (auto& submesh : m_submeshes) {
			submesh.Draw(transform, shader, entityId);
		}
	}

	void Mesh::LoadModel(const std::string& path) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		}
		m_directory = path.substr(0, path.find_last_of('/'));

		uint32_t submeshIndex = 0;

		ProcessNode(scene->mRootNode, scene, submeshIndex);
	}

	void Mesh::ProcessNode(aiNode* node, const aiScene* scene, uint32_t& submeshIndex) {
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_submeshes.push_back(ProcessMesh(mesh, scene, submeshIndex));
			submeshIndex++;
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			ProcessNode(node->mChildren[i], scene, submeshIndex);
		}
	}

	SubMesh Mesh::ProcessMesh(aiMesh* mesh, const aiScene* scene, uint32_t& submeshIndex) {
		std::vector<MeshVertex> vertices;
		std::vector<uint32_t> indices;

		for (size_t i = 0; i < mesh->mNumVertices; ++i) {
			MeshVertex vertex;
			glm::vec3 vector;

			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.position = vector;

			if (mesh->HasNormals()) {
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.normal = vector;
			}

			if (mesh->mTextureCoords[0]) {
				glm::vec2 vec2;
				vec2.x = mesh->mTextureCoords[0][i].x;
				vec2.y = mesh->mTextureCoords[0][i].y;
				vertex.texcoord = vec2;
			}
			else {
				vertex.texcoord = glm::vec2(0.0f, 0.0f);
			}

			if (mesh->HasTangentsAndBitangents()) {
				// TODO
				// tangent
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				// vertex.Tangent = vector;
				// bitangent
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;
				// vertex.Bitangent = vector;
			}
			else {

			}
			vertex.entityId = -1;
			vertices.push_back(vertex);
		}

		for (size_t i = 0; i < mesh->mNumFaces; ++i) {
			auto face = mesh->mFaces[i];
			for (size_t j = 0; j < face.mNumIndices; ++j) {
				indices.push_back(face.mIndices[j]);
			}
		}

		// TODO: material

		return SubMesh(vertices, indices);
	}

	SubMesh::SubMesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices) : m_vertices(vertices), m_indices(indices) {
		m_vao = VertexArray::Create();
		m_vbo = VertexBuffer::Create(m_vertices.size() * sizeof(MeshVertex));
		m_vbo->SetLayout({
			{ "a_position", ShaderDataType::Float3 },
			{ "a_normal",   ShaderDataType::Float3 },
			{ "a_texcoord", ShaderDataType::Float2 },
			{ "a_color",    ShaderDataType::Float4 },
			{ "a_entityId", ShaderDataType::Int    },
		});
		m_vao->AddVertexBuffer(m_vbo);

		m_ibo = IndexBuffer::Create(m_indices.data(), m_indices.size() * sizeof(uint32_t));
		m_vao->SetIndexBuffer(m_ibo);
	}
	void SubMesh::SetupMesh(int entityId) {
		if (m_entityId != -1)
			return;
		m_vao->Bind();

		for (size_t i = 0; i < m_vertices.size(); ++i) {
			m_vertices[i].entityId = entityId;
			m_vertices[i].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		m_vbo->SetData(m_vertices.data(), m_vertices.size() * sizeof(MeshVertex));
	}

	void SubMesh::Draw(const glm::mat4& transform, std::shared_ptr<Shader> shader, int entityId) {
		SetupMesh(entityId);

		m_vao->Bind();
		shader->Bind();
		shader->SetMat4("u_model", transform);

		RenderCommand::DrawElement(m_vao, m_ibo->Count());
	}
}