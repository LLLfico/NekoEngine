#pragma once

#ifdef NEKO_PLATFORM_WINDOWS
	#ifdef NEKO_DYNAMIC_LINK
		#ifdef NEKO_BUILD_DLL
			#define NEKO_API __declspec(dllexport)
		#else
			#define NEKO_API __declspec(dllimport)
		#endif
	#else
		#define NEKO_API 
	#endif
#else	
	#define NEKO_API 
#endif

#ifdef NEKO_DEBUG
	#define NEKO_ENABLE_ASSERT
#endif


#ifdef NEKO_ENABLE_ASSERT
	#define NEKO_ASSERT(x, ...) { if (!x) {NEKO_ERROR("Assertion Failed : {0}", __VA_ARGS__); __debugbreak(); } }
	#define NEKO_CORE_ASSERT(x, ...) { if (!(x)) {NEKO_CORE_ERROR("Assertion Failed : {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define NEKO_ASSERT(x, ...)
	#define NEKO_CORE_ASSERT(x, ...)
#endif


#define BIT(x) (1 << x)

#define NEKO_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)