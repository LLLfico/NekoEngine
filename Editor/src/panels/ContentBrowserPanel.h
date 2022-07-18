#pragma once

#include <filesystem>

namespace Neko {

	class Texture2D;
	class ContentBrowserPanel {
	public:
		ContentBrowserPanel();

		void OnImGuiRender();
	private:
		std::filesystem::path m_currentDirectory;

		std::shared_ptr<Texture2D> m_directoryIcon;
		std::shared_ptr<Texture2D> m_fileIcon;
	};

}


