#include "pch.h"
#include "Application.h"

#include "events/ApplicationEvent.h"
#include "Log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Input.h"


namespace Neko {


	Application* Application::s_instance = nullptr;

	Application::Application() {
		NEKO_ASSERT(!s_instance, "Application already exists!");
		s_instance = this;

		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallback(NEKO_BIND_EVENT_FN(Application::OnEvent));
	}

	Application::~Application() {
	}

	void Application::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(NEKO_BIND_EVENT_FN(Application::OnWindowClose));
		// NEKO_CORE_TRACE("{0}", e);

		for (auto it = m_layerStack.end(); it != m_layerStack.begin(); ) {
			(*--it)->OnEvent(e);
			if (e.handled)
				break;
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_running = false;
		return true;
	}

	void Application::Run() {
		while (m_running){

			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			for (Layer* layer : m_layerStack) {
				layer->OnUpdate();
			}

			// auto [x, y] = Input::GetMousePosition();
			// NEKO_CORE_TRACE("{0}, {1}", x, y);

			m_window->OnUpdate();
			
		}
	}

	void Application::PushLayer(Layer* layer) {
		m_layerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay) {
		m_layerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

}