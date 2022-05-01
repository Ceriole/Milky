#pragma once

#include <memory>

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

	template<typename T>
	using Ref = std::shared_ptr<T>;

}
