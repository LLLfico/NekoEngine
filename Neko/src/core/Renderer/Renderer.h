#pragma once

#include "core/Core.h"
#include "RendererAPI.h"

namespace Neko {

	class RendererAPI;
	class Camera;
	class Shader;

	class Renderer {
	public:

		struct SceneData {
			glm::mat4 cameraMatrix;
		};

		static void BeginScene(const std::shared_ptr<Camera>& camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		static SceneData* s_sceneData;
	};

}