#include "PropertiesPanel.h"

#include "Editor/EditorUtils.h"

#include <imgui/imgui.h>

namespace Milky {

	PropertiesPanel::PropertiesPanel(const Ref<EditorContext>& context, const std::string& title, const std::string& icon, const std::string& shortcut)
		: EditorPanel(context, title, icon, shortcut)
	{}

	void PropertiesPanel::ShowContent()
	{
		if (m_Context->Selection->Count(SelectionType::Entity) == 1)
		{
			bool entityDeleted = false;
			Entity selectedEntity = m_Context->Selection->GetEntity();
			EditorUtils::ShowEntityHeader(selectedEntity, entityDeleted);
			EditorUtils::ShowComponents(selectedEntity);
			if (entityDeleted)
			{
				m_Context->Selection->Remove(selectedEntity);
				m_Context->ActiveScene->DestroyEntity(selectedEntity);
			}
		}
		else if (m_Context->Selection->Count(SelectionType::Entity) > 1)
		{
			// TODO
			ImGui::TextDisabled("Multiple Entites");
		}
		else
		{
			constexpr const char* noSelectionText = "No Entity Selected!\nSelect an entity in the Scene Hierarchy.";
			ImVec2 windowSize = ImGui::GetWindowSize();
			ImVec2 textSize = ImGui::CalcTextSize(noSelectionText);
			ImVec2 textPos = ImVec2{ (windowSize.x - textSize.x) * 0.5f , (windowSize.y - textSize.y) * 0.5f };
			ImGui::SetCursorPos(textPos);
			ImGui::TextDisabled(noSelectionText);
		}
	}

}
