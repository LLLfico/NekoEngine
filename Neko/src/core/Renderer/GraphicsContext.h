#pragma once

#include "core/Core.h"

namespace Neko {

	class NEKO_API GraphicsContext {
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};

}