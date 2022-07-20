#include "pch.h"
#include "ContentBrowserPanel.h"

#include "core/renderer/Texture.h"
#include <imgui/imgui.h>

namespace Neko {

	extern const std::filesystem::path g_assetPath = "assets"; 

	Neko::ContentBrowserPanel::ContentBrowserPanel() : m_currentDirectory(g_assetPath) {
		m_directoryIcon = Texture2D::Create("resources/icon/contentBrowser/DirectoryIcon.png");
		m_fileIcon = Texture2D::Create("resources/icon/contentBrowser/FileIcon.png");
	}

	void Neko::ContentBrowserPanel::OnImGuiRender() {
		ImGui::Begin("Content Browser");

		if (m_currentDirectory != g_assetPath) {
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
			auto relativePath = std::filesystem::relative(path, g_assetPath);
			std::string filenameString = relativePath.filename().string();

			ImGui::PushID(filenameString.c_str());
			std::shared_ptr<Texture2D> icon = directoryEntry.is_directory() ? m_directoryIcon : m_fileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)icon->GetId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			if (ImGui::BeginDragDropSource()) {
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}
			ImGui::PopStyleColor();

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (directoryEntry.is_directory()) {
					m_currentDirectory /= path.filename();
				}
			}
			ImGui::TextWrapped(filenameString.c_str());
			ImGui::NextColumn();
			ImGui::PopID();
		}
		ImGui::Columns(1);
		ImGui::SliderFloat("Thumbnail Size ", &thumbnailSize, 16, 512);
		ImGui::SliderFloat("Padding ", &padding, 0, 32);

		ImGui::End();
	}



}

