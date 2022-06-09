#pragma once
#ifdef NEKO_PLATFORM_WINDOWS
	
extern Neko::Application* Neko::CreateApplication();

int main(int argc, char** argv) {
	auto app = Neko::CreateApplication();
	app->Run();
	delete app;

}

#endif