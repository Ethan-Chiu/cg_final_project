#pragma once

#include "Ethane/Core/Base.h"
#include "Ethane/Core/Log.h"
#include <filesystem>

#ifdef ETH_ENABLE_ASSERTS

	// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define ETH_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { ETH##type##ERROR(msg, __VA_ARGS__); ETH_DEBUGBREAK(); } }
	#define ETH_INTERNAL_ASSERT_WITH_MSG(type, check, ...) ETH_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define ETH_INTERNAL_ASSERT_NO_MSG(type, check) ETH_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", ETH_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define ETH_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define ETH_INTERNAL_ASSERT_GET_MACRO(...) ETH_EXPAND_MACRO( ETH_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, ETH_INTERNAL_ASSERT_WITH_MSG, ETH_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define ETH_ASSERT(...) ETH_EXPAND_MACRO( ETH_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define ETH_CORE_ASSERT(...) ETH_EXPAND_MACRO( ETH_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define ETH_ASSERT(...)
	#define ETH_CORE_ASSERT(...)
#endif