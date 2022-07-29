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
			submesh.Draw(transform, shader, entityId, this);
		}
	}

	void Mesh::LoadModel(const std::string& path) {
		m_materials.resize(256);
		std::string filepath = std::regex_replace(path, std::regex("\\\\"), "/");

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		}
		m_directory = filepath.substr(0, filepath.find_last_of('/'));

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

			vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

			if (mesh->HasNormals()) {
				vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
			}

			if (mesh->mTextureCoords[0]) {
				vertex.texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			}
			else {
				vertex.texcoord = glm::vec2(0.0f, 0.0f);
			}

			if (mesh->HasTangentsAndBitangents()) {
				// TODO
				// tangent
				glm::vec3 vector;
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vertex.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
				// bitangent
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;
				vertex.bitangent = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
			}
			else {
				vertex.tangent = glm::vec3(0.0f);
				vertex.bitangent = glm::vec3(0.0f);
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
		if (mesh->mMaterialIndex >= 0) {
			auto material = scene->mMaterials[mesh->mMaterialIndex];
			auto diffuseMaps = LoadMaterialTexture(material, aiTextureType_DIFFUSE, submeshIndex);
			// NEKO_CORE_INFO("submesh {0}", submeshIndex);
			for (int i = aiTextureType_NONE; i < aiTextureType_UNKNOWN; i++) {
				size_t cnt = material->GetTextureCount(aiTextureType(i));
				// NEKO_CORE_INFO("type {0} has {1} textures", i, cnt);
				
			}
		}

		return SubMesh(vertices, indices, submeshIndex);
	}

	std::vector<MaterialTexture> Mesh::LoadMaterialTexture(aiMaterial* material, aiTextureType type, uint32_t submeshIndex) {
		std::vector<MaterialTexture> textures;

		for (size_t i = 0; i < material->GetTextureCount(type); i++) {
			aiString str;
			material->GetTexture(type, i, &str);
			bool skip = false;
			for (size_t j = 0; j < m_materials[submeshIndex].GetTexturesNum(); j++) {
				if (strcmp(str.C_Str(), m_materials[submeshIndex].GetTextures()[j].path.data()) == 0) {
					// already has the texture
					textures.push_back(m_materials[submeshIndex].GetTextures()[j]);
					skip = true;
					break;
				}
			}
			if (!skip) {
				MaterialTexture texture;
				auto filename = std::string(str.C_Str());
				filename = m_directory + '/' + filename;
				texture.texture = Texture2D::Create(filename);
				switch (type) {
					case aiTextureType_DIFFUSE: {
						texture.type = TextureType::Albedo;
						m_materials[submeshIndex].SetAlbedoMap(texture.texture);
						break;
					}
					default: {
						NEKO_CORE_TRACE("Unknown texture type {0}", type);
					}
				}
				texture.path = str.C_Str();
				textures.push_back(texture);
				m_materials[submeshIndex].AddTexture(texture);
			}
		}
		return textures;
	}

	SubMesh::SubMesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices, int submeshIndex) : m_vertices(vertices), m_indices(indices), m_submeshIndex(submeshIndex) {
		m_vao = VertexArray::Create();
		m_vbo = VertexBuffer::Create(m_vertices.size() * sizeof(MeshVertex));
		m_vbo->SetLayout({
			{ "a_position", ShaderDataType::Float3 },
			{ "a_normal",   ShaderDataType::Float3 },
			{ "a_texcoord", ShaderDataType::Float2 },
			{ "a_tangent",  ShaderDataType::Float3 },
			{ "a_bitangent",ShaderDataType::Float3 },
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
		}
		m_vbo->SetData(m_vertices.data(), m_vertices.size() * sizeof(MeshVertex));
	}

	void SubMesh::Draw(const glm::mat4& transform, std::shared_ptr<Shader> shader, int entityId, Mesh* mesh) {
		SetupMesh(entityId);

		m_vao->Bind();
		shader->Bind();
		shader->SetMat4("u_model", transform);

		auto material = mesh->GetMaterial(m_submeshIndex);
		constexpr uint32_t diffuse_slot = 3;
		if (material.m_useAlbedo) {
			material.m_albedoMap->Bind(diffuse_slot);
		}
		else {
			unsigned char color[4];
			for (size_t i = 0; i < 4; i++)
				color[i] = (unsigned char)(material.m_color[i] * 255.0f);
			material.m_albedoColor->SetData(color, 4 * sizeof(unsigned char));
			material.m_albedoColor->Bind(diffuse_slot);
		}
		shader->SetInt("u_albedoMap", diffuse_slot);

		RenderCommand::DrawElement(m_vao, m_ibo->Count());
	}
}