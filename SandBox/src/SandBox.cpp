#include <Neko.h>

#include "imgui/imgui.h"

class ExampleLayer : public Neko::Layer {
public:
	ExampleLayer() : Layer("Example") {
	}

	void OnUpdate() override {
		// NEKO_INFO("ExampleLayer::Update");

		if (Neko::Input::IsKeyPressed(NEKO_KEY_TAB)) {
			NEKO_TRACE("Tab key is pressed!");
		}
	}

	void OnImGuiRender() override {
		ImGui::Begin("Test");
		ImGui::Text("Hello world");
		ImGui::End();
	}

	void OnEvent(Neko::Event& event) override {
		// NEKO_TRACE("{0}", event);
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