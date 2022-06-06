#include "SettingsPanel.h"

#include <imgui/imgui.h>

namespace Milky {

	SettingsPanel::SettingsPanel(const Ref<EditorContext>& context, const std::string& title, const std::string& icon, const std::string& shortcut)
		: EditorPanel(context, title, icon, shortcut)
	{}

	void SettingsPanel::ShowContent()
	{
		ImGui::Checkbox("Physics Colliders", &m_Context->DrawColliders);
	}

}
