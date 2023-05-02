#include "ethpch.h"
#include "Window.h"

#ifdef ETH_PLATFORM_WINDOWS
	#include "Ethane/Platform/Windows/WindowsWindow.h"
#endif

#ifdef ETH_PLATFORM_MACOS
	#include "Ethane/Platform/MacOS/MacosWindow.h"
#endif

namespace Ethane
{
	Scope<Window> Window::Create(const WindowProps& props)
	{
	#ifdef ETH_PLATFORM_WINDOWS
			return CreateScope<WindowsWindow>(props);
	#elif defined(ETH_PLATFORM_MACOS)
			return CreateScope<MacosWindow>(props);
	#else
			ETH_CORE_ASSERT(false, "Unknown platform!");
			return nullptr;
	#endif
	}

}