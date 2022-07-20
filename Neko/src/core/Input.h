#pragma once

#include "core/Core.h"

#include "core/KeyCodes.h"
#include "core/MouseCodes.h"

namespace Neko {

	class NEKO_API Input {
	protected:
		Input() = default;
	public:
		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;
		virtual ~Input() = default;

		static bool IsKeyPressed(KeyCode key) { return s_instance->IsKeyPressedImpl(key); }
		static bool IsMouseButtonPressed(MouseCode button) { return s_instance->IsMouseButtonPressedImpl(button); }
		static float GetMouseX() { return s_instance->GetMouseXImpl(); }
		static float GetMouseY() { return s_instance->GetMouseYImpl(); }
		static std::pair<float, float> GetMousePosition() { return s_instance->GetMousePositionImpl(); }

		static std::unique_ptr<Input> Create();
	protected:
		virtual bool IsKeyPressedImpl(KeyCode key) = 0;
		virtual bool IsMouseButtonPressedImpl(MouseCode button) = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
		virtual std::pair<float, float> GetMousePositionImpl() = 0;

	private:
		static std::unique_ptr<Input> s_instance;
	};

}