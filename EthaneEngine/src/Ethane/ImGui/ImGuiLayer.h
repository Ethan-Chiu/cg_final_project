#pragma once

#include "Ethane/Core/Layer.h"

namespace Ethane {

	class ImGuiLayer : public Layer
	{
	public:
		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void BlockEvents(bool block) = 0;

		void SetDarkThemeColors();


		static ImGuiLayer* Create();
	};
}