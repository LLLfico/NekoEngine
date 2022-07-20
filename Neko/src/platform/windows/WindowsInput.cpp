#include "pch.h"
#include "WindowsInput.h"

#include "core/Application.h"

#include <GLFW/glfw3.h>

namespace Neko {

	bool WindowsInput::IsKeyPressedImpl(KeyCode key) {
		auto window = static_cast<GLFWwindow*>(Application::GetCurrent().GetWindow().GetNativeWindow());

		auto state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	bool WindowsInput::IsMouseButtonPressedImpl(MouseCode button) {
		auto window = static_cast<GLFWwindow*>(Application::GetCurrent().GetWindow().GetNativeWindow());

		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
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
