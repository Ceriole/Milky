#pragma once

#include "Milky/Core/Base.h"
#include "Milky/Core/Log.h"
#include <filesystem>

#ifdef ML_ENABLE_ASSERTS
	// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define ML_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { ML##type##ERROR(msg, __VA_ARGS__); ML_DEBUGBREAK(); } }
	#define ML_INTERNAL_ASSERT_WITH_MSG(type, check, ...) ML_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define ML_INTERNAL_ASSERT_NO_MSG(type, check) ML_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", ML_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define ML_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define ML_INTERNAL_ASSERT_GET_MACRO(...) ML_EXPAND_MACRO( ML_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, ML_INTERNAL_ASSERT_WITH_MSG, ML_INTERNAL_ASSERT_NO_MSG) )
			
	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define ML_ASSERT(...) ML_EXPAND_MACRO( ML_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define ML_CORE_ASSERT(...) ML_EXPAND_MACRO( ML_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define ML_ASSERT(...)
	#define ML_CORE_ASSERT(...)
#endif
