#include "ethpch.h"
#include "Application.h"
#include "Ethane/Core/Log.h"

#include <GLFW/glfw3.h>
#include "Ethane/Renderer/Renderer.h"
#include "Ethane/Asset/AssetManager.h"
#include "Ethane/Systems/ResourceSystem.h"
#include "Ethane/Systems/TextureSystem.h"
#include "Ethane/Systems/ShaderSystem.h"

namespace Ethane
{
#define BIND_EVENT_FUNCTION(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name, ApplicationCommandLineArgs args)
		: m_CommandLineArgs(args)
	{
		ETH_PROFILE_FUNCTION();

		ETH_CORE_ASSERT(!s_Instance, "Application already exists!")
		s_Instance = this;

		m_Window = Window::Create(WindowProps(name, 400, 300));
		m_Window->SetEventCallback(BIND_EVENT_FUNCTION(OnEvent));
		m_Window->SetVSync(false);
        
        RendererConfig config = {
            m_Window->GetWidth(),
            m_Window->GetHeight(),
            glm::vec2(m_Window->GetWidth()/400, m_Window->GetHeight()/300)
        };
        Renderer::Init(m_Window->GetGraphicsContext(), config);
        
        ResourceSystem::Init();
        
        TextureSystem::Init();
        
        ShaderSystem::Init();
        
        AssetManager::Init();
        
        m_ImGuiLayer = ImGuiLayer::Create();
        PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		ETH_PROFILE_FUNCTION();
	}

	void Application::PushLayer(Layer* layer)
	{
		ETH_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		ETH_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::OnEvent(Event& e)
	{
		ETH_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNCTION(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNCTION(OnWindowResize));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}
	}

	void Application::Run()
	{
		ETH_PROFILE_FUNCTION();

//        uint64_t frameCount = 0;
//        float frameTime = 0;
        
		while (true)
		{
			ETH_PROFILE_SCOPE("RunLoop");

			float time = glfwGetTime(); //plateform GetTime()
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

//            frameCount++;
//            frameTime += timestep;
//            ETH_CORE_INFO("fps: {0}", 1/(frameTime/frameCount));
            
			m_Window->PollEvent();
			if (!m_Running) break;

			if (!m_Minimized)
			{
				if (m_Window->BeginFrame())
				{
                    Renderer::BeginFrame();
					{
						ETH_PROFILE_SCOPE("LayerStack OnUpdate");

						for (Layer* layer : m_LayerStack)
							layer->OnUpdate(timestep);
					}
                    
                    if (m_ImGuiLayer)
                    {
                        m_ImGuiLayer->Begin();
                        {
                            ETH_PROFILE_SCOPE("LayerStack OnImGuiRender");

                            for (Layer* layer : m_LayerStack)
                                layer->OnImGuiRender();
                        }
                        m_ImGuiLayer->End();
                    }

                    Renderer::EndFrame();
					m_Window->EndFrame();
				}
			}
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		Close();

        // shutdown all layers first
        m_LayerStack.Detach();
        
        AssetManager::Shutdown();
        
        ShaderSystem::Shutdown();
        
        TextureSystem::Shutdown();
        
        ResourceSystem::Shutdown();
        
        Renderer::Shutdown();
        
		m_Window->Shutdown();

		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		ETH_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			ETH_CORE_INFO("App minimized");
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		m_Window->OnResize(e.GetWidth(), e.GetHeight());
		return false;
	}
}
