#include <Neko.h>

class ExampleLayer : public Neko::Layer {
public:
	ExampleLayer() : Layer("Example") {}

	void OnUpdate() override {
		NEKO_INFO("ExampleLayer::Update");

		if (Neko::Input::IsKeyPressed(NEKO_KEY_TAB)) {
			NEKO_TRACE("Tab key is pressed!");
		}
	}

	void OnEvent(Neko::Event& event) override {
		// NEKO_TRACE("{0}", event);
	}
};

class SandBox : public Neko::Application {
public:
	SandBox() {
		PushLayer(new ExampleLayer());
		PushOverlay(new Neko::ImGuiLayer());
	}
	~SandBox() {}
};


Neko::Application* Neko::CreateApplication() {
	return new SandBox();
}