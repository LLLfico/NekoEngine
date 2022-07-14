#pragma once

namespace Neko {

	class Scene;

	class SceneSerializer {
	public:
		SceneSerializer(const std::shared_ptr<Scene>& scene);

		void Serialize(const std::string& filePath);
		void SerializeRuntime(const std::string& filePath);

		bool Deserialize(const std::string& filePath);
		bool DeserializeRuntime(const std::string& filePath);
	private:
		std::shared_ptr<Scene> m_scene;
	};

}


