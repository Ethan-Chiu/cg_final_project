#pragma once

#include <glm/glm.hpp>
#include "AABB.h"

namespace Ethane::Math {

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);

    AABB SurroundingBox(AABB box0, AABB box1);

}
