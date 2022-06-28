#pragma once

#include "core/Core.h"

namespace Neko {

	enum class RendererAPI {
		None = 0,
		OpenGL = 1,
	};

	class Renderer {
	public:
		inline static RendererAPI GetAPI() { return s_api; }
	private:
		static RendererAPI s_api;
	};

}