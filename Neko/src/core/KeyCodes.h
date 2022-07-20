#pragma once

// From glfw3.h

namespace Neko {

	typedef enum class KeyCode : uint16_t {
		/* Printable keys */
		Space          =   32,
		Apostrophe     =   39,  /* ' */
		Comma          =   44,  /* , */
		Minus          =   45,  /* - */
		Period         =   46,  /* . */
		Slash          =   47,  /* / */
		D0             =   48,
		D1             =   49,
		D2             =   50,
		D3             =   51,
		D4             =   52,
		D5             =   53,
		D6             =   54,
		D7             =   55,
		D8             =   56,
		D9             =   57,
		Semicolon      =   59,  /* ; */
		Equal          =   61,  /* = */
		A              =   65,
		B              =   66,
		C              =   67,
		D              =   68,
		E              =   69,
		F              =   70,
		G              =   71,
		H              =   72,
		I              =   73,
		J              =   74,
		K              =   75,
		L              =   76,
		M              =   77,
		N              =   78,
		O              =   79,
		P              =   80,
		Q              =   81,
		R              =   82,
		S              =   83,
		T              =   84,
		U              =   85,
		V              =   86,
		W              =   87,
		X              =   88,
		Y              =   89,
		Z              =   90,
		LeftBracket    =   91,  /* [ */
		Backslash      =   92,  /* \ */
		RightBracket   =   93,  /* ] */
		GraveAccent    =   96,  /* ` */
		World1         =   161, /* non-us #1 */
		World2         =   162, /* non-US #2 */

		/* Function keys */
		Escape         =   256,
		Enter          =   257,
		Tab            =   258,
		Backspace      =   259,
		Insert         =   260,
		Delete         =   261,
		Right          =   262,
		Left           =   263,
		Down           =   264,
		Up             =   265,
		PageUp         =   266,
		PageDown       =   267,
		Home           =   268,
		End            =   269,
		CapsLock       =   280,
		ScrollLock     =   281,
		NumLock        =   282,
		PrintScreen    =   283,
		Pause          =   284,
		F1             =   290,
		F2             =   291,
		F3             =   292,
		F4             =   293,
		F5             =   294,
		F6             =   295,
		F7             =   296,
		F8             =   297,
		F9             =   298,
		F10            =   299,
		F11            =   300,
		F12            =   301,
		F13            =   302,
		F14            =   303,
		F15            =   304,
		F16            =   305,
		F17            =   306,
		F18            =   307,
		F19            =   308,
		F20            =   309,
		F21            =   310,
		F22            =   311,
		F23            =   312,
		F24            =   313,
		F25            =   314,

		/* Keypad */
		KP0            =  320,
		KP1            =  321,
		KP2            =  322,
		KP3            =  323,
		KP4            =  324,
		KP5            =  325,
		KP6            =  326,
		KP7            =  327,
		KP8            =  328,
		KP9            =  329,
		KPdecimal      =  330,
		KPdivide       =  331,
		KPmultiply     =  332,
		KPsubtract     =  333,
		KPadd          =  334,
		KPenter        =  335,
		KPequal        =  336,
		LeftShift      =  340,
		LeftControl    =  341,
		LeftAlt        =  342,
		LeftSuper      =  343,
		RightShift     =  344,
		RightControl   =  345,
		RightAlt       =  346,
		RightSuper     =  347,
		Menu           =  348,
	} Key;

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode) {
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}

// From glfw3.h

/* Printable keys */
#define NEKO_KEY_SPACE              ::Neko::Key::Space         
#define NEKO_KEY_APOSTROPHE         ::Neko::Key::Apostrophe     /* ' */
#define NEKO_KEY_COMMA              ::Neko::Key::Comma          /* , */
#define NEKO_KEY_MINUS              ::Neko::Key::Minus          /* - */
#define NEKO_KEY_PERIOD             ::Neko::Key::Period         /* . */
#define NEKO_KEY_SLASH              ::Neko::Key::Slash          /* / */
#define NEKO_KEY_0                  ::Neko::Key::D0             
#define NEKO_KEY_1                  ::Neko::Key::D1             
#define NEKO_KEY_2                  ::Neko::Key::D2             
#define NEKO_KEY_3                  ::Neko::Key::D3             
#define NEKO_KEY_4                  ::Neko::Key::D4             
#define NEKO_KEY_5                  ::Neko::Key::D5             
#define NEKO_KEY_6                  ::Neko::Key::D6             
#define NEKO_KEY_7                  ::Neko::Key::D7             
#define NEKO_KEY_8                  ::Neko::Key::D8             
#define NEKO_KEY_9                  ::Neko::Key::D9             
#define NEKO_KEY_SEMICOLON          ::Neko::Key::Semicolon      /* ; */
#define NEKO_KEY_EQUAL              ::Neko::Key::Equal          /* = */
#define NEKO_KEY_A                  ::Neko::Key::A             
#define NEKO_KEY_B                  ::Neko::Key::B             
#define NEKO_KEY_C                  ::Neko::Key::C             
#define NEKO_KEY_D                  ::Neko::Key::D             
#define NEKO_KEY_E                  ::Neko::Key::E             
#define NEKO_KEY_F                  ::Neko::Key::F             
#define NEKO_KEY_G                  ::Neko::Key::G             
#define NEKO_KEY_H                  ::Neko::Key::H             
#define NEKO_KEY_I                  ::Neko::Key::I             
#define NEKO_KEY_J                  ::Neko::Key::J             
#define NEKO_KEY_K                  ::Neko::Key::K             
#define NEKO_KEY_L                  ::Neko::Key::L             
#define NEKO_KEY_M                  ::Neko::Key::M             
#define NEKO_KEY_N                  ::Neko::Key::N             
#define NEKO_KEY_O                  ::Neko::Key::O             
#define NEKO_KEY_P                  ::Neko::Key::P             
#define NEKO_KEY_Q                  ::Neko::Key::Q             
#define NEKO_KEY_R                  ::Neko::Key::R             
#define NEKO_KEY_S                  ::Neko::Key::S             
#define NEKO_KEY_T                  ::Neko::Key::T             
#define NEKO_KEY_U                  ::Neko::Key::U             
#define NEKO_KEY_V                  ::Neko::Key::V             
#define NEKO_KEY_W                  ::Neko::Key::W             
#define NEKO_KEY_X                  ::Neko::Key::X             
#define NEKO_KEY_Y                  ::Neko::Key::Y             
#define NEKO_KEY_Z                  ::Neko::Key::Z             
#define NEKO_KEY_LEFT_BRACKET       ::Neko::Key::LeftBracket   /* [ */
#define NEKO_KEY_BACKSLASH          ::Neko::Key::Backslash      /* \ */
#define NEKO_KEY_RIGHT_BRACKET      ::Neko::Key::RightBracket  /* ] */
#define NEKO_KEY_GRAVE_ACCENT       ::Neko::Key::GraveAccent   /* ` */
#define NEKO_KEY_WORLD_1            ::Neko::Key::World1        /* non-US #1 */
#define NEKO_KEY_WORLD_2            ::Neko::Key::World2        /* non-US #2 */

/* Function keys */
#define NEKO_KEY_ESCAPE             ::Neko::Key::Escape         
#define NEKO_KEY_ENTER              ::Neko::Key::Enter          
#define NEKO_KEY_TAB                ::Neko::Key::Tab            
#define NEKO_KEY_BACKSPACE          ::Neko::Key::Backspace      
#define NEKO_KEY_INSERT             ::Neko::Key::Insert         
#define NEKO_KEY_DELETE             ::Neko::Key::Delete         
#define NEKO_KEY_RIGHT              ::Neko::Key::Right          
#define NEKO_KEY_LEFT               ::Neko::Key::Left           
#define NEKO_KEY_DOWN               ::Neko::Key::Down           
#define NEKO_KEY_UP                 ::Neko::Key::Up             
#define NEKO_KEY_PAGE_UP            ::Neko::Key::PageUp        
#define NEKO_KEY_PAGE_DOWN          ::Neko::Key::PageDown      
#define NEKO_KEY_HOME               ::Neko::Key::Home           
#define NEKO_KEY_END                ::Neko::Key::End            
#define NEKO_KEY_CAPS_LOCK          ::Neko::Key::CapsLock      
#define NEKO_KEY_SCROLL_LOCK        ::Neko::Key::ScrollLock    
#define NEKO_KEY_NUM_LOCK           ::Neko::Key::NumLock       
#define NEKO_KEY_PRINT_SCREEN       ::Neko::Key::PrintScreen   
#define NEKO_KEY_PAUSE              ::Neko::Key::Pause          
#define NEKO_KEY_F1                 ::Neko::Key::F1             
#define NEKO_KEY_F2                 ::Neko::Key::F2             
#define NEKO_KEY_F3                 ::Neko::Key::F3             
#define NEKO_KEY_F4                 ::Neko::Key::F4             
#define NEKO_KEY_F5                 ::Neko::Key::F5             
#define NEKO_KEY_F6                 ::Neko::Key::F6             
#define NEKO_KEY_F7                 ::Neko::Key::F7             
#define NEKO_KEY_F8                 ::Neko::Key::F8             
#define NEKO_KEY_F9                 ::Neko::Key::F9             
#define NEKO_KEY_F10                ::Neko::Key::F10            
#define NEKO_KEY_F11                ::Neko::Key::F11            
#define NEKO_KEY_F12                ::Neko::Key::F12            
#define NEKO_KEY_F13                ::Neko::Key::F13            
#define NEKO_KEY_F14                ::Neko::Key::F14            
#define NEKO_KEY_F15                ::Neko::Key::F15            
#define NEKO_KEY_F16                ::Neko::Key::F16            
#define NEKO_KEY_F17                ::Neko::Key::F17            
#define NEKO_KEY_F18                ::Neko::Key::F18            
#define NEKO_KEY_F19                ::Neko::Key::F19            
#define NEKO_KEY_F20                ::Neko::Key::F20            
#define NEKO_KEY_F21                ::Neko::Key::F21            
#define NEKO_KEY_F22                ::Neko::Key::F22            
#define NEKO_KEY_F23                ::Neko::Key::F23            
#define NEKO_KEY_F24                ::Neko::Key::F24            
#define NEKO_KEY_F25                ::Neko::Key::F25            
#define NEKO_KEY_KP_0               ::Neko::Key::KP0           
#define NEKO_KEY_KP_1               ::Neko::Key::KP1           
#define NEKO_KEY_KP_2               ::Neko::Key::KP2           
#define NEKO_KEY_KP_3               ::Neko::Key::KP3           
#define NEKO_KEY_KP_4               ::Neko::Key::KP4           
#define NEKO_KEY_KP_5               ::Neko::Key::KP5           
#define NEKO_KEY_KP_6               ::Neko::Key::KP6           
#define NEKO_KEY_KP_7               ::Neko::Key::KP7           
#define NEKO_KEY_KP_8               ::Neko::Key::KP8           
#define NEKO_KEY_KP_9               ::Neko::Key::KP9           
#define NEKO_KEY_KP_DECIMAL         ::Neko::Key::KPdecimal     
#define NEKO_KEY_KP_DIVIDE          ::Neko::Key::KPdivide      
#define NEKO_KEY_KP_MULTIPLY        ::Neko::Key::KPmultiply    
#define NEKO_KEY_KP_SUBTRACT        ::Neko::Key::KPsubtract    
#define NEKO_KEY_KP_ADD             ::Neko::Key::KPadd         
#define NEKO_KEY_KP_ENTER           ::Neko::Key::KPenter       
#define NEKO_KEY_KP_EQUAL           ::Neko::Key::KPequal       
#define NEKO_KEY_LEFT_SHIFT         ::Neko::Key::LeftShift     
#define NEKO_KEY_LEFT_CONTROL       ::Neko::Key::LeftControl   
#define NEKO_KEY_LEFT_ALT           ::Neko::Key::LeftAlt       
#define NEKO_KEY_LEFT_SUPER         ::Neko::Key::LeftSuper     
#define NEKO_KEY_RIGHT_SHIFT        ::Neko::Key::RightShift    
#define NEKO_KEY_RIGHT_CONTROL      ::Neko::Key::RightControl  
#define NEKO_KEY_RIGHT_ALT          ::Neko::Key::RightAlt      
#define NEKO_KEY_RIGHT_SUPER        ::Neko::Key::RightSuper    
#define NEKO_KEY_MENU               ::Neko::Key::Menu           

