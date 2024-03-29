#pragma once

#include "pch.h"

#include "Core.h"
#include "events/Event.h"

namespace Neko {

	struct WindowInfos {
		std::string title = "Neko Engine";
		unsigned int width = 1600;
		unsigned int height = 900;

		WindowInfos(const std::string& _title = "Neko Engine", unsigned int _width = 1600, unsigned int _height = 900) : title(_title), width(_width), height(_height) {}
	};

	class NEKO_API Window {
	public:
		using EventCallbackFunction = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		// window attributes
		virtual void SetEventCallback(const EventCallbackFunction& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static std::unique_ptr<Window> Create(const WindowInfos& infos = WindowInfos());
	};

}