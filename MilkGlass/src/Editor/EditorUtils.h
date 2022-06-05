#pragma once

#include "Milky/Scene/Scene.h"

#include "SelectionContext.h"

#define ML_PAYLOAD_TYPE_ASSET	"_ML_ASSET"

namespace Milky::EditorUtils {

	void ShowNewEntityMenu(const Ref<Scene>& scene);
	void ShowEntityMenuItems(Entity entity, bool& entityDeleted);
	void ShowAddComponentMenuItems(Entity entity); 
	void ShowEntityHeader(Entity entity, bool& entityDeleted);
	void ShowComponents(Entity entity);

	template<typename Comp, typename UIFunction>
	void ShowComponent(const std::string& name, Entity entity, UIFunction uiFunction, bool deleteable = true);

	glm::vec3 GetAverageVector(const std::vector<glm::vec3>& vectors);
}
