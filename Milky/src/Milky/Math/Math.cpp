#include "mlpch.h"
#include "Math.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Milky::Math {

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& o_translation, glm::vec3& o_rotation, glm::vec3& o_scale)
	{
		// From glm::decomponse in matrix_decompose.inl

		using namespace glm;
		using T = float;

		mat4 localMatrix(transform);

		// Normalize the matrix
		if (epsilonEqual(localMatrix[3][3], static_cast<float>(0), epsilon<T>()))
			return false;

		// Isolate perspective
		if (
			epsilonNotEqual(localMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(localMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(localMatrix[2][3], static_cast<T>(0), epsilon<T>()))
		{
			// Clear perspecitve from matrix
			localMatrix[0][3] = localMatrix[1][3] = localMatrix[2][3] = static_cast<T>(0);
			localMatrix[3][3] = static_cast<T>(1);
		}

		// Translation
		o_translation = vec3(localMatrix[3]);
		localMatrix[3] = vec4(0, 0, 0, localMatrix[3].w);

		vec3 Row[3], Pdum3;

		for (length_t i = 0; i < 3; ++i)
			for (length_t j = 0; j < 3; ++j)
				Row[i][j] = localMatrix[i][j];

		// Compute scale factor and normalize first row
		o_scale.x = length(Row[0]);
		Row[0] = detail::scale(Row[0], static_cast<T>(1));
		o_scale.y = length(Row[1]);
		Row[1] = detail::scale(Row[1], static_cast<T>(1));
		o_scale.z = length(Row[2]);
		Row[2] = detail::scale(Row[2], static_cast<T>(1));

#if 0
		// At this point, the matrix (in rows[]) is orthonormal.
		// Check for a coordinate system flip.  If the determinant
		// is -1, then negate the matrix and the scaling factors.
		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (dot(Row[0], Pdum3) < 0)
		{
			for (length_t i = 0; i < 3; i++)
			{
				Scale[i] *= static_cast<T>(-1);
				Row[i] *= static_cast<T>(-1);
			}
		}
#endif

		o_rotation.y = asin(-Row[0][2]);
		if (cos(o_rotation.y) != 0)
		{
			o_rotation.x = atan2(Row[1][2], Row[2][2]);
			o_rotation.z = atan2(Row[0][1], Row[0][0]);
		}
		else
		{
			o_rotation.x = atan2(-Row[2][0], Row[1][1]);
			o_rotation.z = 0;
		}

		return true;
	}
}
