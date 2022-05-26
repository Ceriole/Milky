#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <filesystem>

#include <IconsFontAwesome5.h>

namespace Milky {

	ContentBrowserPanel::ContentBrowserPanel(const Ref<EditorData>& context, const std::string& title, const std::string& icon, const std::string& shortcut)
		: EditorPanel(context, title, icon, shortcut)
	{
		m_CurrentDirectory = m_AssetPath;
		m_DirectoryIcon = Texture2D::Create("assets/textures/editor/folder_open.png");
		m_FileIcon = Texture2D::Create("assets/textures/editor/file.png");
	};

	void ContentBrowserPanel::ShowContent()
	{
		std::string dirPath = m_CurrentDirectory.string();
		ImGui::Text(dirPath.c_str());

		static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		if (ImGui::BeginTable("files", columnCount))
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::TableNextColumn();
			if (m_CurrentDirectory != m_AssetPath)
			{
				ImGui::ImageButton((ImTextureID)m_DirectoryIcon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					m_CurrentDirectory = m_CurrentDirectory.parent_path();
				ImGui::TextWrapped("..");
				ImGui::TableNextColumn();
			}

			for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
			{
				const auto& path = directoryEntry.path();
				auto relativePath = std::filesystem::relative(path, m_AssetPath);
				std::string filenameString = relativePath.filename().string();
				Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
				ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					if (directoryEntry.is_directory())
						m_CurrentDirectory /= path.filename();
				ImGui::TextWrapped(filenameString.c_str());
				ImGui::TableNextColumn();
			}
			ImGui::PopStyleColor();
			ImGui::EndTable();
		}
	}

}
