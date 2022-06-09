#pragma once
#ifdef NEKO_PLATFORM_WINDOWS
	
extern Neko::Application* Neko::CreateApplication();

int main(int argc, char** argv) {

	Neko::Log::Init();
	NEKO_CORE_WARN("Initialized Log~");
	int a = 5;
	NEKO_INFO("Hello~ Var = {0}", a);


	auto app = Neko::CreateApplication();
	app->Run();
	delete app;

}

#endif