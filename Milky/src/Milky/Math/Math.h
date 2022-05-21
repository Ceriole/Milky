#pragma once

#include <glm/glm.hpp>

namespace Milky::Math {

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& o_translation, glm::vec3& o_rotation, glm::vec3& o_scale);

}
