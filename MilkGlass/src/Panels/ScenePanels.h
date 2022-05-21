#pragma once

#include "Milky/Scene/Scene.h"
#include "Milky/Core/Base.h"
#include "Milky/Scene/Entity.h"

namespace Milky {

	class ScenePanels
	{
	public:
		ScenePanels() = default;
		ScenePanels(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void SetShown(bool shown)
		{
			m_ShowSceneHierarchyPanel = shown;
			m_ShowPropertiesPanel = shown;
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
	private:
		bool m_ShowSceneHierarchyPanel = true;
		bool m_ShowPropertiesPanel = true;
		Ref<Scene> m_Context;
		Entity m_SelectedEntity;
	};

}
