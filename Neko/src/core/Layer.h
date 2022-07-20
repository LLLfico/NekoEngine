#pragma once

#include "Core.h"
#include "events/Event.h"
#include "TimeStep.h"

namespace Neko {

	class NEKO_API Layer {
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(TimeStep t) {}
		virtual void OnImGuiRender() {} // iiyari, is needed to add an specific function to base class ?
		virtual void OnEvent(Event& event) {}

		const std::string& GetName() const { return m_debugName; }
	private:
		std::string m_debugName;  // just for debug, not for search or hash
	};
}


