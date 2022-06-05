#include "ToolbarPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Milky {

	ToolbarPanel::ToolbarPanel(const Ref<EditorContext>& context, const std::string& title, const std::string& icon, const std::string& shortcut)
		: EditorPanel(context, title, icon, shortcut)
	{
		ShowInMenu = false;

		m_IconPlay = Texture2D::Create("Resources/Icons/play.png");
		m_IconStop = Texture2D::Create("Resources/Icons/stop.png");
		m_IconPause = Texture2D::Create("Resources/Icons/pause.png");
	}

	void ToolbarPanel::ShowContent()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, buttonHovered.w * 0.5f));
		auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, buttonHovered.w * 0.5f));

		float size = ImGui::GetContentRegionAvail().y;
		Ref<Texture2D> icon = m_Context->State == EditorState::Edit ? m_IconPlay : m_IconStop;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
		if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			if (m_Context->State == EditorState::Edit)
				m_Context->OnScenePlay();
			else if (m_Context->State == EditorState::Play)
				m_Context->OnSceneStop();
		}

		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(2);
	}

	int ToolbarPanel::GetWindowFlags()
	{
		return ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	}

}
