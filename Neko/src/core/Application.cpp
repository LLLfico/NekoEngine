#include "pch.h"
#include "Application.h"

#include "events/ApplicationEvent.h"
#include "Log.h"
#include "imgui/ImGuiLayer.h"
#include "Input.h"
#include "Time.h"

#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Camera.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


namespace Neko {


	Application* Application::s_instance = nullptr;

	Application::Application() {
		NEKO_ASSERT(!s_instance, "Application already exists!");
		s_instance = this;

		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallback(NEKO_BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();

		m_imguiLayer = new ImGuiLayer();
		PushOverlay(m_imguiLayer);
	}

	Application::~Application() {
	}

	void Application::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(NEKO_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(NEKO_BIND_EVENT_FN(Application::OnWindowResize));
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

	bool Application::OnWindowResize(WindowResizeEvent& e) {
		if (e.GetWidth() == 0 || e.GetHeight() == 0) {
			//ÃÔ»ó
			m_minimized = true;
			return false;
		}

		m_minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return true;
	}

	void Application::Run() {
		while (m_running){
			float curTime = (float)glfwGetTime();
			Time timeStep = curTime - m_lastFrameTime;
			m_lastFrameTime = curTime;
			

			for (Layer* layer : m_layerStack) {
				layer->OnUpdate(timeStep);
			}

			m_imguiLayer->Begin();
			for (Layer* layer : m_layerStack) {
				layer->OnImGuiRender();
			}
			m_imguiLayer->End();

			m_window->OnUpdate();
			
		}
	}

	void Application::PushLayer(Layer* layer) {
		m_layerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* overlay) {
		m_layerStack.PushOverlay(overlay);
	}

}