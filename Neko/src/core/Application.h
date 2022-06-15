#pragma once

#include "Core.h"
#include "Window.h"



namespace Neko {

	class WindowCloseEvent;

	class NEKO_API Application {
	public:
		Application();
		~Application();

		void OnEvent(Event& e);
		void Run();
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		std::unique_ptr<Window> m_window;
		bool m_running = true;
	};

	// to be defined in client
	Application* CreateApplication();
}


