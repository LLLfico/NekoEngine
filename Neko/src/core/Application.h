#pragma once

#include "Core.h"
#include "Window.h"

namespace Neko {

	class NEKO_API Application {
	public:
		Application();
		~Application();

		void Run();
	private:
		std::unique_ptr<Window> m_window;
		bool m_running = true;
	};

	// to be defined in client
	Application* CreateApplication();
}


