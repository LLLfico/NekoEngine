#include "pch.h"
#include "WindowsWindow.h"

#include "core/events/ApplicationEvent.h"
#include "core/events/MouseEvent.h"
#include "core/events/KeyEvent.h"

#include "platform/opengl/OpenGLContext.h"

namespace Neko {

	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* desc) {
		NEKO_CORE_ERROR("GLFW Error {0} : {1}", error, desc);
	}

	WindowsWindow::WindowsWindow(const WindowInfos& infos) {
		Init(infos);
	}

	WindowsWindow::~WindowsWindow() {
		Shutdown();
	}

	void WindowsWindow::Init(const WindowInfos& infos) {
		m_data.title = infos.title;
		m_data.width = infos.width;
		m_data.height = infos.height;

		NEKO_CORE_INFO("Create Window {0} {1},{2}", infos.title, infos.width, infos.height);

		if (s_GLFWWindowCount == 0) {
			NEKO_CORE_INFO("Initializing GLFW");
			int success = glfwInit();
			NEKO_CORE_ASSERT(success, "Could not initialized GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		m_window = glfwCreateWindow((int)infos.width, (int)infos.height, infos.title.c_str(), nullptr, nullptr);
		++s_GLFWWindowCount;

		m_context = std::make_unique<OpenGLContext>(m_window);
		m_context->Init();

		glfwWindowHint(GLFW_SAMPLES, 4);

		glfwSetWindowUserPointer(m_window, &m_data);
		SetVSync(true);

		// set glfw callbacks
		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.width = width;
			data.height = height;

			WindowResizeEvent event(width, height);
			data.callback(event);
			
		});

		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.callback(event);
		});

		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			switch (action) {
				case GLFW_PRESS: {
					KeyPressedEvent event(static_cast<KeyCode>(key), 0);
					data.callback(event);
					break;
				}
				case GLFW_RELEASE: {
					KeyReleasedEvent event(static_cast<KeyCode>(key));
					data.callback(event);
					break;
				}
				case GLFW_REPEAT: {
					KeyPressedEvent event(static_cast<KeyCode>(key), 1);
					data.callback(event);
					break;
				}
			}
		});

		glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int codepoint) {
			// codepoint is an unicode character
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(static_cast<KeyCode>(codepoint));
			data.callback(event);
		});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			switch (action) {
				case GLFW_PRESS: {
					MouseButtonPressedEvent event(static_cast<MouseCode>(button));
					data.callback(event);
					break;
				}
				case GLFW_RELEASE: {
					MouseButtonReleasedEvent event(static_cast<MouseCode>(button));
					data.callback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xoffset, (float)yoffset);
			data.callback(event);
		});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xpos, (float)ypos);
			data.callback(event);
		});
	}

	void WindowsWindow::Shutdown() {
		glfwDestroyWindow(m_window);
		--s_GLFWWindowCount;
		if (s_GLFWWindowCount == 0) {
			NEKO_CORE_INFO("Terminating GLFW");
			glfwTerminate();
		}
	}

	void WindowsWindow::OnUpdate() {
		glfwPollEvents();
		m_context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled) {
		if (enabled) {
			glfwSwapInterval(1);
		}
		else {
			glfwSwapInterval(0);
		}
	}

	bool WindowsWindow::IsVSync() const {
		return m_data.vsync;
	}
}