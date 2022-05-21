#pragma once

#include "Milky/Scene/Scene.h"
#include "Milky/Core/Base.h"
#include "Milky/Scene/Entity.h"

#include <IconsFontAwesome5.h>

#define SCENE_HIERARCHY_TITLE		"Scene Hierarchy"
#define SCENE_HIERARCHY_ICON		ICON_FA_LIST
#define SCENE_HIERARCHY_TAB_TITLE	SCENE_HIERARCHY_ICON " " SCENE_HIERARCHY_TITLE
#define PROPERTIES_TITLE			"Properties"
#define PROPERTIES_ICON				ICON_FA_WRENCH
#define PROPERTIES_TAB_TITLE		PROPERTIES_ICON " " PROPERTIES_TITLE

namespace Milky {

	class ScenePanels
	{
	public:
		ScenePanels() = default;
		ScenePanels(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void SetShown(bool shown)
		{
			ShowSceneHierarchyPanel = shown;
			ShowPropertiesPanel = shown;
		}

		void OnImguiRender();
		void ShowWindowMenuItems();
		void ShowNewEntityMenu();

		Entity GetSelectedEntity() const { return m_SelectedEntity; };
	private:
		void DrawAddComponentMenuItems(Entity entity);
		void ShowEntityHeader(Entity entity, bool& entityDeleted);
		void ShowEntityNode(Entity entity);
		void ShowComponents(Entity entity);
	public:
		bool ShowSceneHierarchyPanel = true;
		bool ShowPropertiesPanel = true;
	private:
		Ref<Scene> m_Context;
		Entity m_SelectedEntity;
	};

}
