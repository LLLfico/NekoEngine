#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"

namespace Neko {

	class WindowCloseEvent;
	class ImGuiLayer;
	class Shader;

	class NEKO_API Application {
	public:
		Application();
		~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() { return *m_window; }

		inline static Application& GetCurrent() { return *s_instance; }

		unsigned int m_vao, m_vbo, m_ibo;
		std::unique_ptr<Shader> m_shader;
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_window;
		ImGuiLayer* m_imguiLayer;
		bool m_running = true;
		LayerStack m_layerStack;

		static Application* s_instance;
	};

	// to be defined in client
	Application* CreateApplication();
}


