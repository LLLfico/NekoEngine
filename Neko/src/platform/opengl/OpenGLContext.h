#pragma once

#include "core/renderer/GraphicsContext.h"

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


