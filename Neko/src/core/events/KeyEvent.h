#pragma once

#include "Event.h"
#include "core/Input.h"


namespace Neko {

	class NEKO_API KeyEvent : public Event {
	public:
		KeyCode GetKeyCode() const { return m_keycode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(KeyCode keycode) : m_keycode(keycode) {}

		KeyCode m_keycode;
	};

	class NEKO_API KeyPressedEvent : public KeyEvent {
	public:
		KeyPressedEvent(KeyCode keycode, int repeatCount) : KeyEvent(keycode), m_repeatCount(repeatCount) {}

		int GetRepeatCount() const { return m_repeatCount; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_keycode << "(" << m_repeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	protected:
		int m_repeatCount;
	};

	class NEKO_API KeyReleasedEvent : public KeyEvent {
	public:
		KeyReleasedEvent(KeyCode keycode) : KeyEvent(keycode) {}

		std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_keycode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class NEKO_API KeyTypedEvent : public KeyEvent {
	public:
		KeyTypedEvent(KeyCode keycode) : KeyEvent(keycode) {}

		std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_keycode;
			return ss.str();
		}
		EVENT_CLASS_TYPE(KeyTyped)
	};
}