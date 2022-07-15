#include "pch.h"
#include "Math.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Neko::Math {
	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale) {

		using namespace glm;
		using T = float;
		// copy from glm decompose 
		mat4 localMatrix(transform);

		// normalize
		if (epsilonEqual(localMatrix[3][3], static_cast<float>(0), epsilon<T>()))
			return false;

		// perspective
		if (epsilonEqual(localMatrix[0][3], static_cast<float>(0), epsilon<T>()) ||
			epsilonEqual(localMatrix[1][3], static_cast<float>(0), epsilon<T>()) ||
			epsilonEqual(localMatrix[2][3], static_cast<float>(0), epsilon<T>())) {

			// clear perspective partition
			localMatrix[0][3] = localMatrix[1][3] = localMatrix[2][3] = static_cast<T>(0);
			localMatrix[3][3] = static_cast<T>(1);
		}

		// translation
		outTranslation = vec3(localMatrix[3]);
		// clear translation partition
		localMatrix[3] = vec4(0, 0, 0, localMatrix[3].w);

		// scale and shear
		vec3 row[3], pdum3;
		for (length_t i = 0; i < 3; i++)
			for (length_t j = 0; j < 3; j++)
				row[i][j] = localMatrix[i][j];

		outScale.x = length(row[0]);
		outScale.y = length(row[1]);
		outScale.z = length(row[2]);

		// clear scale partition, make row matrix orthographic
		row[0] = detail::scale(row[0], static_cast<T>(1));
		row[1] = detail::scale(row[1], static_cast<T>(1));
		row[2] = detail::scale(row[2], static_cast<T>(1));

		#if 0
		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (dot(Row[0], Pdum3) < 0) {
			for (length_t i = 0; i < 3; i++) {
				scale[i] *= static_cast<T>(-1);
				Row[i] *= static_cast<T>(-1);
			}
		}
		#endif

		outRotation.y = asin(-row[0][2]);
		if (cos(outRotation.y) != 0) {
			outRotation.x = atan2(row[1][2], row[2][2]);
			outRotation.z = atan2(row[0][1], row[0][0]);
		}
		else {
			outRotation.x = atan2(-row[2][0], row[1][1]);
			outRotation.z = 0;
		}
		
		return true;
	}
}