#include "pch.h"
#include "Application.h"

#include "events/ApplicationEvent.h"
#include "Log.h"
#include "imgui/ImGuiLayer.h"
#include "Input.h"
#include "Renderer/Shader.h"

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

		m_imguiLayer = new ImGuiLayer();
		PushOverlay(m_imguiLayer);

		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);
		glGenBuffers(1, &m_ibo);

		float vertices[] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f, 0.5f, 0.0f,
		};

		unsigned int indices[] = {
			0, 1, 2,
		};
		
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);	
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
			}
		)";

		m_shader.reset(new Shader(vertexSrc, fragmentSrc));
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

			m_shader->Bind();
			glBindVertexArray(m_vao);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
			m_shader->Unbind();

			for (Layer* layer : m_layerStack) {
				layer->OnUpdate();
			}

			m_imguiLayer->Begin();
			for (Layer* layer : m_layerStack) {
				layer->OnImGuiRender();
			}
			m_imguiLayer->End();

			// auto [x, y] = Input::GetMousePosition();
			// NEKO_CORE_TRACE("{0}, {1}", x, y);

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