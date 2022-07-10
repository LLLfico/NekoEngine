#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"

namespace Neko {

	class WindowCloseEvent;
	class WindowResizeEvent;
	class ImGuiLayer;
	class Shader;
	class VertexBuffer;
	class IndexBuffer;
	class VertexArray;

	class NEKO_API Application {
	public:
		Application();
		~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() { return *m_window; }

		void Close();

		inline static Application& GetCurrent() { return *s_instance; }
		
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		std::unique_ptr<Window> m_window;
		ImGuiLayer* m_imguiLayer;
		bool m_running = true;
		bool m_minimized = false;
		LayerStack m_layerStack;
		float m_lastFrameTime = 0.0f;

		static Application* s_instance;
	};

	// to be defined in client
	Application* CreateApplication();
}


