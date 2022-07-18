#pragma once

#include <filesystem>

namespace Neko {

	class ContentBrowserPanel {
	public:
		ContentBrowserPanel();

		void OnImGuiRender();
	private:
		std::filesystem::path m_currentDirectory;
	};

}


