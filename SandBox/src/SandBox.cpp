#include <Neko.h>

class SandBox : public Neko::Application {
public:
	SandBox() {}
	~SandBox() {}
};


Neko::Application* Neko::CreateApplication() {
	return new SandBox();
}