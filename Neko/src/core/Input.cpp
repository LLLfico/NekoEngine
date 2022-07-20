#include "pch.h"
#include "Input.h"

#ifdef NEKO_PLATFORM_WINDOWS
	#include "platform/windows/WindowsInput.h"
#endif // NEKO_PLATFORM_WINDOWS


namespace Neko {

	std::unique_ptr<Input> Input::s_instance = Input::Create();

	std::unique_ptr<Input> Input::Create() {
		#ifdef NEKO_PLATFORM_WINDOWS
			return std::make_unique<WindowsInput>();
		#else
			NEKO_CORE_ASSERT(false, "Unsupported platform!");
			return nullptr;
		#endif // NEKO_PLATFORM_WINDOWS
	}

}