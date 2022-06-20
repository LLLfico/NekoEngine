#include "pch.h"
#include "WindowsInput.h"

#include "core/Application.h"

#include <GLFW/glfw3.h>

namespace Neko {

	Input* Input::s_instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int keycode) {
		auto window = static_cast<GLFWwindow*>(Application::GetCurrent().GetWindow().GetNativeWindow());

		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	bool WindowsInput::IsMouseButtonPressedImpl(int button) {
		auto window = static_cast<GLFWwindow*>(Application::GetCurrent().GetWindow().GetNativeWindow());

		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}
	float WindowsInput::GetMouseXImpl() {
		auto [x, y] = GetMousePositionImpl();
		return x;
	}
	float WindowsInput::GetMouseYImpl() {
		auto [x, y] = GetMousePositionImpl();
		return y;
	}
	std::pair<float, float> WindowsInput::GetMousePositionImpl() {
		auto window = static_cast<GLFWwindow*>(Application::GetCurrent().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return { (float)xpos, (float)ypos };
	}
}
