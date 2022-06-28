#include "pch.h"
#include "Application.h"

#include "events/ApplicationEvent.h"
#include "Log.h"
#include "imgui/ImGuiLayer.h"
#include "Input.h"
#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"

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

		m_vao.reset(VertexArray::Create());

		float vertices[] = {
			-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 0.5f, 0.0f,	   0.0f, 0.0f, 1.0f, 1.0f,
		};

		unsigned int indices[] = {
			0, 1, 2,
		};
		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
		
		BufferLayout layout = {
				{"a_position", ShaderDataType::Float3},
				{"a_color", ShaderDataType::Float4},
		};
		vertexBuffer->SetLayout(layout);
		m_vao->AddVertexBuffer(vertexBuffer);

		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices)));
		m_vao->SetIndexBuffer(indexBuffer);

		float squareVertices[] = {
			-0.75f, -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};
		m_squareVAO.reset(VertexArray::Create());
		m_squareVAO->Bind();
		std::shared_ptr<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ "a_Position", ShaderDataType::Float3,  }
			});
		m_squareVAO->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices)));
		m_squareVAO->SetIndexBuffer(squareIB);


		std::string vertexSrc = R"(
			#version 460 core
			
			layout(location = 0) in vec3 a_position;
			layout(location = 1) in vec4 a_color;
			out vec3 v_position;
			out vec4 v_color;
			void main()
			{
				v_position = a_position;
				v_color = a_color;
				gl_Position = vec4(a_position, 1.0);	
			}
		)";

		std::string fragmentSrc = R"(
			#version 460 core
			
			layout(location = 0) out vec4 color;
			in vec3 v_position;
			in vec4 v_color;
			void main()
			{
				color = vec4(v_position * 0.5 + 0.5, 1.0);
				color = v_color;
			}
		)";

		m_shader.reset(new Shader(vertexSrc, fragmentSrc));

		std::string blueShaderVertexSrc = R"(
			#version 460 core
			
			layout(location = 0) in vec3 a_Position;
			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);	
			}
		)";

		std::string blueShaderFragmentSrc = R"(
			#version 460 core
			
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			void main()
			{
				color = vec4(0.1, 0.3, 0.8, 1.0);
			}
		)";

		m_blueShader = std::make_shared<Shader>(blueShaderVertexSrc, blueShaderFragmentSrc);


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

			m_blueShader->Bind();
			m_squareVAO->Bind();
			glDrawElements(GL_TRIANGLES, m_squareVAO->GetIndexBuffer()->Count(), GL_UNSIGNED_INT, nullptr);
			m_squareVAO->Unbind();
			m_blueShader->Unbind();

			m_shader->Bind();
			m_vao->Bind();
			glDrawElements(GL_TRIANGLES, m_vao->GetIndexBuffer()->Count(), GL_UNSIGNED_INT, nullptr);

			for (Layer* layer : m_layerStack) {
				layer->OnUpdate();
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