#pragma once

#include "Ethane/ImGui/ImGuiLayer.h"
#include "Ethane/Renderer/Vulkan/VulkanContext.h"
#include "../VulkanRenderTarget.h"
#include "Ethane/Events/ApplicationEvent.h"

namespace Ethane{

	class VulkanImGuiLayer : public ImGuiLayer
	{
	public:
		VulkanImGuiLayer();
		~VulkanImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

        bool OnResize(WindowResizeEvent& e);
        
		virtual void Begin() override;
		virtual void End() override;

		virtual void BlockEvents(bool block) override { m_BlockEvents = block; }

	private: 
		void Cleanup();

	private:
		bool m_BlockEvents = true;
        bool m_NeedResize = false;
        uint32_t m_Width, m_Height;
		float m_Time = 0.0f;

        VkDescriptorPool m_DescriptorPool;
        Scope<VulkanRenderTarget> m_Target;
	};

}
