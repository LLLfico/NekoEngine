#include "pch.h"
#include "Application.h"

#include "events/ApplicationEvent.h"
#include "Log.h"

#include <GLFW/glfw3.h>

namespace Neko {
	Application::Application() {
		m_window = std::unique_ptr<Window>(Window::Create());
	}

	Application::~Application() {

	}

	void Application::Run() {

		WindowResizeEvent e(1280, 720);
		if (e.IsInCategory(EventCategoryApplication)) {
			NEKO_TRACE(e);
		}
		if (e.IsInCategory(EventCategoryInput)) {
			NEKO_TRACE(e);
		}

		while (true){
			if (m_running) {

				glClearColor(1, 0, 1, 1);
				glClear(GL_COLOR_BUFFER_BIT);

				m_window->OnUpdate();
			}
		}
	}

}