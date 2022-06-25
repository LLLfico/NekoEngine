#pragma once

#include "core/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Neko {

	class OpenGLContext : public GraphicsContext {
	public:
		OpenGLContext(GLFWwindow* window);

		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_window;
	};

}


