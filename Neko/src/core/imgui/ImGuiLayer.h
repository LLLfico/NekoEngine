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
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:

	private:
		float m_time = 0.0f;

	};

}


