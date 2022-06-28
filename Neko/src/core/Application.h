#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"

namespace Neko {

	class WindowCloseEvent;
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

		inline static Application& GetCurrent() { return *s_instance; }


		uint32_t t_vao, t_vbo, t_ibo;
		std::shared_ptr<Shader> m_shader;
		std::shared_ptr<VertexArray> m_vao;
		std::shared_ptr<VertexBuffer> m_vertexBuffer;

		std::shared_ptr<Shader> m_blueShader;
		std::shared_ptr<VertexArray> m_squareVAO;
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


