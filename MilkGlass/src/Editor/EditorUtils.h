#pragma once

#include "Milky/Scene/Scene.h"

#include "SelectionContext.h"

namespace Milky::EditorUtils {

	void HandleEntitySelection(const Ref<SelectionContext>& selection, Entity entity);

	void ShowNewEntityMenu(const Ref<Scene>& scene);
	void ShowEntityMenuItems(Entity entity, bool& entityDeleted);

	void ShowAddComponentMenuItems(Entity entity);

	void ShowEntityHeader(Entity entity, bool& entityDeleted);

	void ShowComponents(Entity entity);

	template<typename Comp, typename UIFunction>
	void ShowComponent(const std::string& name, Entity entity, UIFunction uiFunction, bool deleteable = true);
}
