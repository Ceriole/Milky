#pragma once

#ifdef ML_PLATFORM_WINDOWS
#ifdef ML_DYNAMIC_LINK
	#ifdef ML_BUILD_DLL
		#define MILKY_API __declspec(dllexport)
	#else
		#define MILKY_API __declspec(dllimport)
	#endif
#else
	#define MILKY_API
#endif
#else
	#error Milky only supports Windows!
#endif

#ifdef ML_DEBUG
	#define ML_ENABLE_ASSERTS
#endif

#ifdef ML_ENABLE_ASSERTS
	#define ML_ASSERT(x, ...) { if(!(x)) { ML_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); }}
	#define ML_CORE_ASSERT(x, ...) { if(!(x)) { ML_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); }}
#else
	#define ML_ASSERT(x, ...)
	#define ML_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define ML_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)