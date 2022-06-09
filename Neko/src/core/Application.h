#pragma once

#include "Core.h"

namespace Neko {

	class NEKO_API Application {
	public:
		Application();
		~Application();

		void Run();
	};

	// to be defined in client
	Application* CreateApplication();
}


