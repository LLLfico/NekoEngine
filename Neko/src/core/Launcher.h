#pragma once
#ifdef NEKO_PLATFORM_WINDOWS
	
extern Neko::Application* Neko::CreateApplication();

int main(int argc, char** argv) {

	Neko::Log::Init();

	// NEKO_PROFILE_BEGIN_SESSION("Start up", "startup.json");
	auto app = Neko::CreateApplication();
	//NEKO_PROFILE_END_SESSION();

	app->Run();

	delete app;

}

#endif