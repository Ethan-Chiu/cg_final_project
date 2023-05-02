#include "ethpch.h"

#include "WindowsWindow.h"

#include "Ethane/Events/ApplicationEvent.h"
#include "Ethane/Events/KeyEvent.h"
#include "Ethane/Events/MouseEvent.h"

// test
#include "Ethane/Renderer/Renderer.h"

// TODO: remove this 
#include <vulkan/vulkan.h>

namespace Ethane {

	static bool s_WindowInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		ETH_CORE_ERROR("GLFW Error ({0}), {1}", error, description);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		ETH_PROFILE_FUNCTION();

		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		ETH_PROFILE_FUNCTION();

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		ETH_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);


		if (!s_WindowInitialized)
		{
			//glfwTerminate on system shutdown
			int success = glfwInit();
			ETH_CORE_ASSERT(success, "Couldn't initialize GLFW");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_WindowInitialized = true;
		}

		// test
		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);

		m_Context = GraphicsContext::Create(m_Window);
		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		//Set GLFW callback
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				data.EventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});

	}

	void WindowsWindow::Shutdown()
	{
		ETH_PROFILE_FUNCTION();

		glfwDestroyWindow(m_Window);
		m_Context->Shutdown();
	}

	bool WindowsWindow::BeginFrame()
	{
		return m_Context->BeginFrame();
	}

	void WindowsWindow::EndFrame()
	{
		ETH_PROFILE_FUNCTION();
		m_Context->SwapBuffers();
	}

	void WindowsWindow::PollEvent()
	{
		glfwPollEvents();
	}

	void WindowsWindow::SetVSync(bool enable)
	{
		ETH_PROFILE_FUNCTION();
		if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
		{
			if (enable)
				glfwSwapInterval(1);
			else
				glfwSwapInterval(0);
			m_Data.VSync = enable;
		}
	}

	void WindowsWindow::OnResize(uint32_t width, uint32_t height)
	{
		m_Context->OnResize(width, height);
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

}