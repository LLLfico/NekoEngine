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

		static void Init();
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(const Camera& camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		static std::unique_ptr<SceneData> s_sceneData;
	};

}