#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"



namespace Neko {

	class WindowCloseEvent;

	class NEKO_API Application {
	public:
		Application();
		~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_window;
		bool m_running = true;
		LayerStack m_layerStack;
	};

	// to be defined in client
	Application* CreateApplication();
}


