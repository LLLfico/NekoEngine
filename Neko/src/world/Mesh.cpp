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
		shader->Bind();
		shader->SetBool("u_animated", m_animated);
		if (m_animated) {
			m_animator.UpdateAnimation(1.0f);
			auto boneTransforms = m_animator.GetFinalBoneMatrices();
			for (size_t i = 0; i < boneTransforms.size(); i++) {
				shader->SetMat4("u_finalBoneMatrices[" + std::to_string(i) + "]", boneTransforms[i]);
			}
		}
		
		for (auto& submesh : m_submeshes) {
			submesh.Draw(transform, shader, entityId, this);
		}
	}

	void Mesh::LoadModel(const std::string& path) {
		m_materials.resize(256);
		std::string filepath = std::regex_replace(path, std::regex("\\\\"), "/");

		Assimp::Importer importer;
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
		const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		}
		m_directory = filepath.substr(0, filepath.find_last_of('/'));

		uint32_t submeshIndex = 0;

		if (scene->HasAnimations()) {
			m_animated = true;
			ProcessNode(scene->mRootNode, scene, submeshIndex);
			m_animation = Animation::Animation(filepath, this);
			m_animator = Animator::Animator(&m_animation);
		}
		else {
			ProcessNode(scene->mRootNode, scene, submeshIndex);
		}
		m_materials.resize(submeshIndex);
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

			SetVertexBoneDataDefault(vertex);

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
			NEKO_CORE_INFO("submesh {0}", submeshIndex);
			for (int i = aiTextureType_NONE; i < aiTextureType_UNKNOWN; i++) {
				size_t cnt = material->GetTextureCount(aiTextureType(i));
				NEKO_CORE_INFO("type {0} has {1} textures", i, cnt);
				
			}
		}

		if (m_animated) {
			ExtractBoneWeightForVertices(vertices, mesh, scene);
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

	void Mesh::SetVertexBoneDataDefault(MeshVertex& vertex) {
		for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
			vertex.boneIds[i] = -1;
			vertex.weights[i] = 0.0f;
		}
	}

	void Mesh::ExtractBoneWeightForVertices(std::vector<MeshVertex>& vertices, aiMesh* mesh, const aiScene* scene) {
		for (size_t boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++) {
			int boneId = -1;
			std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
			if (m_boneInfoMap.count(boneName)) {
				boneId = m_boneInfoMap[boneName].id;
			}
			else {
				BoneInfo boneInfo;
				boneInfo.id = m_boneCounter;
				boneId = m_boneCounter;
				m_boneCounter++;
				auto& from = mesh->mBones[boneIndex]->mOffsetMatrix;
				glm::mat4 to;
				//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
				to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
				to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
				to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
				to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
				boneInfo.offset = to;
				m_boneInfoMap[boneName] = boneInfo;
			}
			NEKO_CORE_ASSERT(boneId == -1, "Bone import failed!");
			auto weights = mesh->mBones[boneIndex]->mWeights;
			int numWeights = mesh->mBones[boneIndex]->mNumWeights;

			for (size_t weightIndex = 0; weightIndex < numWeights; weightIndex++) {
				int vertexId = weights[weightIndex].mVertexId;
				float weight = weights[weightIndex].mWeight;
				NEKO_CORE_ASSERT(vertexId <= vertices.size(), "Unexpected vertex id");
				for (size_t i = 0; i < MAX_BONE_INFLUENCE; i++) {
					// select a slot add id and weight
					if (vertices[vertexId].boneIds[i] < 0) {
						vertices[vertexId].weights[i] = weight;
						vertices[vertexId].boneIds[i] = boneId;
						break;
					}
				}
			}
		}
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

			{ "a_boneIds",  ShaderDataType::Int4   },
			{ "a_weights",  ShaderDataType::Float4 },
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
		constexpr uint32_t albedoSlot = 3;
		constexpr uint32_t metallicSlot = 4;
		constexpr uint32_t roughnessSlot = 5;
		constexpr uint32_t aoSlot = 6;
		if (material.m_useAlbedoMap) {
			material.m_albedoMap->Bind(albedoSlot);
		}
		else {
			unsigned char color[4];
			for (size_t i = 0; i < 4; i++)
				color[i] = (unsigned char)(material.m_color[i] * 255.0f);
			material.m_albedoSprite->SetData(color, 4 * sizeof(unsigned char));
			material.m_albedoSprite->Bind(albedoSlot);
		}
		shader->SetInt("u_albedoMap", albedoSlot);

		if (material.m_useMetallicMap) {
			material.m_metallicMap->Bind(metallicSlot);
		}
		else {
			unsigned char color[4];
			for (size_t i = 0; i < 3; i++)
				color[i] = (unsigned char)(material.m_metalllic * 255.0f);
			color[3] = 255.0f;
			material.m_metallicSprite->SetData(color, 4 * sizeof(unsigned char));
			material.m_metallicSprite->Bind(metallicSlot);
		}
		shader->SetInt("u_metallicMap", metallicSlot);

		if (material.m_useRoughnessMap) {
			material.m_roughnessMap->Bind(roughnessSlot);
		}
		else {
			unsigned char color[4];
			for (size_t i = 0; i < 3; i++)
				color[i] = (unsigned char)(material.m_roughness * 255.0f);
			color[3] = 255.0f;
			material.m_roughnessSprite->SetData(color, 4 * sizeof(unsigned char));
			material.m_roughnessSprite->Bind(roughnessSlot);
		}
		shader->SetInt("u_roughnessMap", roughnessSlot);

		if (material.m_useAOMap) {
			material.m_aoMap->Bind(aoSlot);
		}
		else {
			unsigned char color[4];
			for (size_t i = 0; i < 3; i++)
				color[i] = (unsigned char)(material.m_ao * 255.0f);
			color[3] = 255.0f;
			material.m_aoSprite->SetData(color, 4 * sizeof(unsigned char));
			material.m_aoSprite->Bind(aoSlot);
		}
		shader->SetInt("u_aoMap", aoSlot);

		RenderCommand::DrawElement(m_vao, m_ibo->Count());
	}
}