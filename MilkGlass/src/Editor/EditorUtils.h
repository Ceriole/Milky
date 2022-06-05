#pragma once

#include "Milky/Scene/Scene.h"

#include "EditorContext.h"

#define ML_PAYLOAD_TYPE_ASSET	"_ML_ASSET"

namespace Milky::EditorUtils {

	void ShowNewEntityMenu(const Ref<EditorContext>& context);
	void ShowEntityMenuItems(const Ref<EditorContext>& context, Entity entity, bool& entityDeleted);
	void ShowMultiEntityMenuItems(const Ref<EditorContext>& context, bool& entitiesDeleted);
	void ShowAddComponentMenuItems(const Ref<EditorContext>& context, Entity entity);
	void ShowEntityHeader(const Ref<EditorContext>& context, Entity entity, bool& entityDeleted);
	void ShowComponents(const Ref<EditorContext>& context, Entity entity);

	template<typename Comp, typename UIFunction>
	void ShowComponent(const Ref<EditorContext>& context, const std::string& name, Entity entity, UIFunction uiFunction, bool deleteable = true);

	glm::vec3 GetAverageVector(const std::vector<glm::vec3>& vectors);
}
