#include "StatsPanel.h"

#include "Milky/Renderer/Renderer2D.h"

#include <imgui/imgui.h>

namespace Milky {

	StatsPanel::StatsPanel(const Ref<EditorContext>& context, const std::string& title, const std::string& icon, const std::string& shortcut)
		: EditorPanel(context, title, icon, shortcut)
	{}

	void StatsPanel::ShowContent()
	{
		auto stats = Renderer2D::GetStats();
		if (ImGui::TreeNodeEx("Renderer2D", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, ImGui::GetStyle().Colors[ImGuiCol_FrameBg]);
			if (ImGui::BeginTable("renderer2DStats", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Draw Calls");
				ImGui::TableNextColumn();
				ImGui::Text("%d", stats.DrawCalls);
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Quads");
				ImGui::TableNextColumn();
				ImGui::Text("%d", stats.QuadCount);
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Vertices");
				ImGui::TableNextColumn();
				ImGui::Text("%d", stats.GetTotalVertexCount());
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Indices");
				ImGui::TableNextColumn();
				ImGui::Text("%d", stats.GetTotalIndexCount());
				ImGui::EndTable();
			}
			ImGui::PopStyleColor();
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Editor", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, ImGui::GetStyle().Colors[ImGuiCol_FrameBg]);
			if (ImGui::BeginTable("editorStats", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Entity Count");
				ImGui::TableNextColumn();
				ImGui::Text("%d", m_Context->ActiveScene->GetNumEntites());

				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				if (m_Context->Selection->Count(SelectionType::Entity) > 1)
					ImGui::Text("Selected Entities");
				else
					ImGui::Text("Selected Entity");

				ImGui::TableNextColumn();
				if (m_Context->Selection->Has())
				{
					std::string selectedText = "";
					switch (m_Context->Selection->Type())
					{
					case SelectionType::Entity:
						auto entities = m_Context->ActiveScene->GetEntities(m_Context->Selection->GetAll());
						for (auto& entity : entities)
							selectedText += entity.GetName() + (entities.back() != entity ? ", " : "");
						break;
						// TODO: Other selection types
					}
					ImGui::Text(selectedText.c_str());
				}
				else
					ImGui::TextDisabled("None");

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Hovered Entity");
				ImGui::TableNextColumn();
				if (m_Context->HoveredEntity)
					ImGui::Text(m_Context->HoveredEntity.GetName().c_str());
				else
					ImGui::TextDisabled("None");
				ImGui::EndTable();
			}
			ImGui::PopStyleColor();
			ImGui::TreePop();
		}
	}
}
