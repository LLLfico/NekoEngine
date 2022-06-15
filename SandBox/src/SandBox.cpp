#include <Neko.h>

class ExampleLayer : public Neko::Layer {
public:
	ExampleLayer() : Layer("Example") {}

	void OnUpdate() override {
		NEKO_INFO("ExampleLayer::Update");
	}

	void OnEvent(Neko::Event& event) override {
		NEKO_TRACE("{0}", event);
	}
};

class SandBox : public Neko::Application {
public:
	SandBox() {
		PushLayer(new ExampleLayer());
	}
	~SandBox() {}
};


Neko::Application* Neko::CreateApplication() {
	return new SandBox();
}