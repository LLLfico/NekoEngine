#pragma once

#include "core/Core.h"
#include "core/Layer.h"

#include "core/events/KeyEvent.h"
#include "core/events/MouseEvent.h"
#include "core/events/ApplicationEvent.h"

namespace Neko {

	class NEKO_API ImGuiLayer : public Layer{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Event& event) override;

	private:
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		bool OnWindowResizeEvent(WindowResizeEvent& e);
		bool OnKeyTypedEvent(KeyTypedEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);

	private:
		float m_time = 0.0f;

	};

}


