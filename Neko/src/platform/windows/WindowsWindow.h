#pragma once
#include "core/Window.h"

#include <GLFW/glfw3.h>

namespace Neko {

	class GraphicsContext;

	class WindowsWindow : public Window {
	public:
		WindowsWindow(const WindowInfos& infos);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		unsigned int GetWidth() const override { return m_data.width; }
		unsigned int GetHeight() const override { return m_data.height; }

		// window attributes
		void SetEventCallback(const EventCallbackFunction& callback) override { m_data.callback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		virtual void* GetNativeWindow() const override { return m_window; }
	private:
		virtual void Init(const WindowInfos& infos);
		virtual void Shutdown();
	private:
		GLFWwindow* m_window;
		std::unique_ptr<GraphicsContext> m_context;

		// convinent to pass data, just cluster specific data
		struct WindowData {
			std::string title;
			unsigned int width, height;
			bool vsync;

			EventCallbackFunction callback;
		};

		WindowData m_data;
	};

}

