#pragma once

#include <memory>

#ifdef ML_DEBUG
	#define ML_ENABLE_ASSERTS
#endif

// Platform detection using predefined macros
#ifdef _WIN32
	// Windows x64/x86
	#ifdef _WIN64
		// Windows x64
		#define ML_PLATFORM_WINDOWS
	#else
		// Windows x86
		#error "x86 builds are not supported!"
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	/* TARGET_OS_MAC exists on all platforms
	 * so we must check all of them (in the below order)
	 * to ensure that we're running on MAC
	 * and not other Apple platforms */
	#if TARGET_IPHONE_SIMULATOR == 1
		#error "IOS simulator is not supported!"
	#elif TARGET_OS_IPHONE == 1
		#define ML_PLATFORM_IOS
		#error "IOS is not supported!"
	#elif TARGET_OS_MAC == 1
		#define ML_PLATFORM_MACOS
		#error "MacOS is not supported!"
	#else
		#error "Unknown Apple platform!"
	#endif	
/* Also check __ANDROID__ before __linux__
 * since Andrioid is based on the linux kernel,
 * it has __linux__ defined. */
#elif defined(__ANDROID__)
	#define ML_PLATFORM_ANDROID
	#error "Android is not supported!"
#elif defined(__linux__)
	#define ML_PLATFORM_LINUX
	#error "Android is not supported!"
#else
	// Unknown compiler/platform
	#error "Unknown platform!"
#endif // End of platform detection

// DLL support
#ifdef ML_PLATFORM_WINDOWS
	#if ML_DYNAMIC_LINK
		#ifdef ML_BUILD_DLL
			#define MILKY_API __declspec(dllexport)
		#else
			#define MILKY_API __declspec(dllimport)
		#endif
	#else
		#define MILKY_API
	#endif
#endif // End of DLL support

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

#define ML_EXPAND_MACRO(x) x
#define ML_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define ML_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

#include "Milky/Core/Log.h"

namespace Milky {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}
