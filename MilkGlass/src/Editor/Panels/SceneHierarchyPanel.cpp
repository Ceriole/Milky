#include "SceneHierarchyPanel.h"

#include "Milky/Core/Input.h"
#include "Milky/Scene/Entity.h"
#include "Milky/Scene/Components/ComponentsBasic.h"

#include "Editor/EditorUtils.h"

#include <imgui/imgui.h>

namespace Milky {

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<EditorData>& context, const std::string& title, const std::string& icon, const std::string& shortcut)
		: EditorPanel(context, title, icon, shortcut)
	{}

	void SceneHierarchyPanel::ShowContent()
	{
		m_Context->ActiveScene->m_Registry.each([&](auto entityID)
			{
				Entity entity{ entityID, m_Context->ActiveScene.get() };
				ShowEntityNode(entity);
			});

		if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered() && !Input::IsCtrlDown())
			m_Context->Selection->Clear(SelectionType::Entity);

		if (ImGui::BeginPopupContextWindow(NULL, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			EditorUtils::ShowNewEntityMenu(m_Context->ActiveScene);
			ImGui::EndPopup();
		}
	}

	void SceneHierarchyPanel::ShowEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = (m_Context->Selection->Has(entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsMouseClicked(0) && ImGui::IsItemHovered())
			EditorUtils::HandleEntitySelection(m_Context->Selection, entity);

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			EditorUtils::ShowEntityMenuItems(entity, entityDeleted);
			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGui::TextDisabled("No Children");
			/* TODO: Entity children and parenting
			for (Entity child : entity.Children)
				DrawEntityNode(child);
			*/
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			m_Context->Selection->Remove(entity);
			m_Context->ActiveScene->DestroyEntity(entity);
		}
	}

}
