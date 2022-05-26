#include "EditorPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Milky {
	
	EditorPanel::EditorPanel(const Ref<EditorData>& context, const std::string& title, const std::string& icon, const std::string& shortcut)
		: m_Title(title), m_Icon(icon), m_Shortcut(shortcut)
	{
		m_TabTitle = m_Icon + "  " + m_Title;
		SetContext(context);
	}
	
	void EditorPanel::OnImGuiRender()
	{
		if (Show)
		{
			if (ImGui::Begin(m_TabTitle.c_str(), &Show))
			{
				CalculateBounds();
				ShowContent();
			}
			ImGui::End();
		}
	}

	void EditorPanel::ShowMenuItem()
	{
		if (ImGui::MenuItemEx(m_Title.c_str(), m_Icon.c_str(), m_Shortcut.c_str(), Show)) ToggleOpen();
	}

	void EditorPanel::CalculateBounds()
	{
		auto minRegion = ImGui::GetWindowContentRegionMin();
		auto maxRegion = ImGui::GetWindowContentRegionMax();
		auto offset = ImGui::GetWindowPos(); // Where ImGui will start rendering for the viewport.
		m_Bounds[0] = { minRegion.x + offset.x, minRegion.y + offset.y };
		m_Bounds[1] = { maxRegion.x + offset.x, maxRegion.y + offset.y };
		m_Hovered = ImGui::IsWindowHovered();
		m_Focused = ImGui::IsWindowFocused();
	}

}
