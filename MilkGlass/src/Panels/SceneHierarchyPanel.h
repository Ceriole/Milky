#pragma once

#include "Milky/Scene/Scene.h"
#include "Milky/Core/Base.h"
#include "Milky/Scene/Entity.h"

namespace Milky {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImguiRender();
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectedEntity;
	};

}
