#pragma once

#ifdef NEKO_PLATFORM_WINDOWS
	#ifdef NEKO_BUILD_DLL
		#define NEKO_API __declspec(dllexport)
	#else
		#define NEKO_API __declspec(dllimport)
	#endif
#else	
	#error neko only support windows now
#endif


#define BIT(x) (1 << x)