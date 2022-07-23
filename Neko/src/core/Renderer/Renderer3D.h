#pragma once

#include "EditorCamera.h"

namespace Neko {

	class VertexArray;
	class Mesh;
	class Renderer3D {
	public:
		static void Init();

		static void BeginScene(const EditorCamera& camera);
		static void DrawMesh(const glm::mat4& transform, std::shared_ptr<Mesh> mesh, int entityId = -1);
		static void EndScene();
	};

}