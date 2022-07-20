#include "pch.h"
#include "Window.h"

#ifdef NEKO_PLATFORM_WINDOWS
	#include "platform/windows/WindowsWindow.h"
#endif // NEKO_PLATFORM_WINDOWS


namespace Neko {

	std::unique_ptr<Window> Window::Create(const WindowInfos& infos) {
		#ifdef NEKO_PLATFORM_WINDOWS
			return std::make_unique<WindowsWindow>(infos);
		#else 
			NEKO_CORE_ASSERT(false, "Unsupported platform!");
			return nullptr;
		#endif
	}

}