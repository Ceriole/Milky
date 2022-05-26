#pragma once

#include "EditorPanel.h"

namespace Milky {

	class SceneHierarchyPanel : public EditorPanel
	{
	public:
		SceneHierarchyPanel(const Ref<EditorData>& context, const std::string& title, const std::string& icon, const std::string& shortcut);
	protected:
		virtual void ShowContent() override;
	private:
		void ShowEntityNode(Entity entity);
	};

}
