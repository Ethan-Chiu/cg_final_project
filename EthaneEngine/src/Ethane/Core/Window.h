#pragma once

#include "ethpch.h"
#include "Ethane/Core/Base.h"
#include "Ethane/Events/Event.h"
#include "Ethane/Renderer/GraphicsContext.h"

namespace Ethane {

	struct WindowProps 
	{
		std::string Title;
		uint32_t ScreenCoordWidth;
		uint32_t ScreenCoordHeight;

		WindowProps(const std::string& title = "Ethane Engine", 
					uint32_t width = 1600,
					uint32_t height = 900)
			: Title(title), ScreenCoordWidth(width), ScreenCoordHeight(height) {}
			
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual bool BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void PollEvent() = 0;

		virtual void OnResize(uint32_t width, uint32_t height) = 0;
		virtual void Shutdown() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enable) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;
        virtual const GraphicsContext* GetGraphicsContext() const = 0;

		static Scope<Window> Create(const WindowProps& props = WindowProps());
	};

}
