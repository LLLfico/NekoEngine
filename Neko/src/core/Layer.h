#pragma once

#include "Core.h"
#include "events/Event.h"

namespace Neko {

	class NEKO_API Layer {
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_debugName; }
	private:
		std::string m_debugName;  // just for debug, not for search or hash
	};
}


