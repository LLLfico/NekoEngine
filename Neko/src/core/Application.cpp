#include "Application.h"

#include "events/ApplicationEvent.h"
#include "Log.h"

namespace Neko {
	Application::Application() {
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

		}
	}

}