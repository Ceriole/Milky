#pragma once

#include "EditorPanel.h"

namespace Milky {

	class PropertiesPanel : public EditorPanel
	{
	public:
		PropertiesPanel(const Ref<EditorContext>& context, const std::string& title, const std::string& icon, const std::string& shortcut);
	protected:
		virtual void ShowContent() override;
	};

}
