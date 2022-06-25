#include "pch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Neko {

	Neko::OpenGLContext::OpenGLContext(GLFWwindow* window) : m_window(window) {
	}

	void Neko::OpenGLContext::Init() {
		glfwMakeContextCurrent(m_window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		NEKO_CORE_ASSERT(status, "Failed to initialize Glad!");

		NEKO_CORE_INFO("OpenGL Info:");
		NEKO_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		NEKO_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		NEKO_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));
	}

	void Neko::OpenGLContext::SwapBuffers() {
		glfwSwapBuffers(m_window);
	}

}

