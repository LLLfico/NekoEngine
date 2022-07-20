#pragma once

#include "core/Input.h"

namespace Neko {

	class WindowsInput : public Input{
	public:

	protected:
		virtual bool IsKeyPressedImpl(KeyCode key) override;
		virtual bool IsMouseButtonPressedImpl(MouseCode button) override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
		virtual std::pair<float, float> GetMousePositionImpl() override;
	private:

	};

}

