#pragma once

#include "EditorCamera.h"

namespace Neko {

	class VertexArray;
	class FrameBuffer;
	class Mesh;
	class Scene;
	class Renderer3D {
	public:
		static void Init();

		static void BeginScene(const EditorCamera& camera);
		static void BindDepthTexture(uint32_t slot);
		static void SetDirectionalLight(const glm::vec3& direction, const glm::vec3& radiance);
		static void SetPointLight(const glm::vec3& position, const glm::vec3& radiance, int index);
		static void SetPointLightNum(int num);
		static void DrawMesh(const glm::mat4& transform, std::shared_ptr<Mesh> mesh, int entityId = -1);
		static void EndScene();

		static std::shared_ptr<FrameBuffer> s_shadowFbo;
	};

}