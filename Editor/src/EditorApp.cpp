#include <Neko.h>
#include <core/Launcher.h>

#include "EditorLayer.h"

namespace Neko {

	class Editor : public Application {
	public:
		Editor() : Application() {
			PushLayer(new EditorLayer());
		}
		~Editor();
	};

	Application* CreateApplication() {
		return new Editor();
	}
}
