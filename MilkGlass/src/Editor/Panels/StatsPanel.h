#pragma once

#include "EditorPanel.h"

namespace Milky {

	class StatsPanel : public EditorPanel
	{
	public:
		StatsPanel(const Ref<EditorData>& context, const std::string& title, const std::string& icon, const std::string& shortcut);
	protected:
		virtual void ShowContent() override;
	};

}
