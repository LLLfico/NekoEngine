#pragma once

namespace Neko {

	typedef enum class MouseCode : uint16_t {
		// From glfw3.h
		Button1       =  0,
		Button2       =  1,
		Button3       =  2,
		Button4       =  3,
		Button5       =  4,
		Button6       =  5,
		Button7       =  6,
		Button8       =  7,
		ButtonLast    =  Button8,
		ButtonLeft    =  Button1,
		ButtonRight   =  Button2,
		ButtonMiddle  =  Button3,
	} Mouse;

	inline std::ostream& operator<<(std::ostream& os, MouseCode mouseCode) {
		os << static_cast<int32_t>(mouseCode);
		return os;
	}
}


// From glfw3.h
#define NEKO_MOUSE_BUTTON_1         ::Neko::Mouse::Button1      
#define NEKO_MOUSE_BUTTON_2         ::Neko::Mouse::Button2      
#define NEKO_MOUSE_BUTTON_3         ::Neko::Mouse::Button3      
#define NEKO_MOUSE_BUTTON_4         ::Neko::Mouse::Button4      
#define NEKO_MOUSE_BUTTON_5         ::Neko::Mouse::Button5      
#define NEKO_MOUSE_BUTTON_6         ::Neko::Mouse::Button6      
#define NEKO_MOUSE_BUTTON_7         ::Neko::Mouse::Button7      
#define NEKO_MOUSE_BUTTON_8         ::Neko::Mouse::Button8      
#define NEKO_MOUSE_BUTTON_LAST      ::Neko::Mouse::ButtonLast   
#define NEKO_MOUSE_BUTTON_LEFT      ::Neko::Mouse::ButtonLeft   
#define NEKO_MOUSE_BUTTON_RIGHT     ::Neko::Mouse::ButtonRight  
#define NEKO_MOUSE_BUTTON_MIDDLE    ::Neko::Mouse::ButtonMiddle 