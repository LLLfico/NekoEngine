#pragma once

#include "pch.h"

#include "Core.h"
#include "events/Event.h"

namespace Neko {

	struct WindowInfos {
		std::string title;
		unsigned int width;
		unsigned int height;

		WindowInfos(const std::string& _title = "Neko Engine", unsigned int _width = 1280, unsigned int _height = 720) : title(_title), width(_width), height(_height) {}
	};

	class NEKO_API Window {
	public:
		using EventCallbackFunction = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		// window attributes
		virtual void SetEventCallback(const EventCallbackFunction& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Window* Create(const WindowInfos& infos = WindowInfos());
	};

}