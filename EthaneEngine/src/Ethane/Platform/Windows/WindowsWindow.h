#pragma once

#include "Ethane/Core/Window.h"
#include "Ethane/Renderer/GraphicsContext.h"
#include <GLFW/glfw3.h>

namespace Ethane {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		bool BeginFrame() override;
		void EndFrame() override;

		void PollEvent() override;

		void OnResize(uint32_t width, uint32_t height) override;
		void Shutdown() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }

		void SetVSync(bool enable);
		bool IsVSync() const;

		inline virtual void* GetNativeWindow() const { return m_Window; };
	
	private:
		virtual void Init(const WindowProps& props);

	private:
		GLFWwindow* m_Window;
		Scope<GraphicsContext> m_Context;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;

	};
}