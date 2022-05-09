#pragma once

#include <memory>

#include "Milky/Core/PlatformDetection.h"

#ifdef ML_DEBUG
	#if defined(ML_PLATFORM_WINDOWS)
		#define ML_DEBUGBREAK() __debugbreak()
	#elif defined(ML_PLATFORM_LINUX)
		#include <signal.h>
		#define ML_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define ML_ENABLE_ASSERTS
#else
	#define ML_DEBUGBREAK()
#endif

#define ML_EXPAND_MACRO(x) x
#define ML_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define ML_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

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

#include "Milky/Core/Log.h"
#include "Milky/Core/Assert.h"
