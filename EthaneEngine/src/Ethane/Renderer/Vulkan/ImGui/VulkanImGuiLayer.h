#pragma once

#include "Ethane/ImGui/ImGuiLayer.h"
#include "Ethane/Renderer/Vulkan/VulkanContext.h"
#include "../VulkanRenderTarget.h"

namespace Ethane{

	class VulkanImGuiLayer : public ImGuiLayer
	{
	public:
		VulkanImGuiLayer();
		~VulkanImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		virtual void Begin() override;
		virtual void End() override;

		virtual void BlockEvents(bool block) override { m_BlockEvents = block; }

	private: 
		void Cleanup();

	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;

        Scope<VulkanRenderTarget> m_Target;
	};

}
