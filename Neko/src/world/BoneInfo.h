#pragma once

#include <glm/glm.hpp>

namespace Neko {

	struct BoneInfo {
		int id;
		glm::mat4 offset;
	};

}