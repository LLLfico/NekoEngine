#include "pch.h"
#include "WindowsWindow.h"

namespace Neko {

	static bool s_GLFWInitialized = false;

	Window* Window::Create(const WindowInfos& infos) {
		return new WindowsWindow(infos);
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

		if (!s_GLFWInitialized) {
			// todo : glfwterminate on system shutdown
			int success = glfwInit();
			NEKO_CORE_ASSERT(success, "Could not initialized GLFW!");

			s_GLFWInitialized = true;
		}

		m_window = glfwCreateWindow((int)infos.width, (int)infos.height, infos.title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_window);
		glfwSetWindowUserPointer(m_window, &m_data);
		SetVSync(true);
	}

	void WindowsWindow::Shutdown() {
		glfwDestroyWindow(m_window);
	}

	void WindowsWindow::OnUpdate() {
		glfwPollEvents();
		glfwSwapBuffers(m_window);
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