#pragma once

#include<memory>

#include "PlatformDetection.h"

// DLL support
/* #ifdef ETH_PLATFORM_WINDOWS
	#if ETH_DYNAMIC_LINK
		#ifdef ETH_BUILD_DLL
			#define ETHANE_API __declspec(dllexport)
		#else
			#define ETHANE_API __declspec(dllimport)
		#endif
	#else
		#define ETHANE_API
	#endif
#else
	#error Windows support only
#endif */

//////////////////////////

#ifdef ETH_DEBUG
	#if defined(ETH_PLATFORM_WINDOWS)
		#define ETH_DEBUGBREAK() __debugbreak()
	#elif defined(ETH_PLATFORM_LINUX)
		#include <signal.h>
		#define ETH_DEBUGBREAK() raise(SIGTRAP)
	#else
		#define ETH_DEBUGBREAK()
		// #error "Platform doesn't support debugbreak yet!"
	#endif
	#define ETH_ENABLE_ASSERTS
#else
	#define ETH_DEBUGBREAK()
#endif

#define ETH_EXPAND_MACRO(x) x
#define ETH_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define ETH_BIND_EVENT_FN(fn)  [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); } 

namespace Ethane {

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

#include "Ethane/Core/Assert.h"
