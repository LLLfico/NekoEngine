#include "pch.h"
#include "ContentBrowserPanel.h"

#include "core/Renderer/Texture.h"
#include <imgui/imgui.h>

namespace Neko {

	static const std::filesystem::path s_assetPath = "assets"; 

	Neko::ContentBrowserPanel::ContentBrowserPanel() : m_currentDirectory(s_assetPath) {
		m_directoryIcon = Texture2D::Create("resources/icon/contentBrowser/DirectoryIcon.png");
		m_fileIcon = Texture2D::Create("resources/icon/contentBrowser/FileIcon.png");
	}

	void Neko::ContentBrowserPanel::OnImGuiRender() {
		ImGui::Begin("Content Browser");

		if (m_currentDirectory != s_assetPath) {
			if (ImGui::Button("<-")) {
				m_currentDirectory = m_currentDirectory.parent_path();
			}
		}

		static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		columnCount = std::max(columnCount, 1);

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_currentDirectory)) {
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, s_assetPath);
			std::string filenameString = relativePath.filename().string();

			std::shared_ptr<Texture2D> icon = directoryEntry.is_directory() ? m_directoryIcon : m_fileIcon;
			ImGui::ImageButton((ImTextureID)icon->GetId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (directoryEntry.is_directory()) {
					m_currentDirectory /= path.filename();
				}
			}
			ImGui::TextWrapped(filenameString.c_str());
			ImGui::NextColumn();
		}
		ImGui::Columns(1);
		ImGui::SliderFloat("Thumbnail Size ", &thumbnailSize, 16, 512);
		ImGui::SliderFloat("Padding ", &padding, 0, 32);

		ImGui::End();
	}



}

