#pragma once

#include <glm/glm.hpp>
#include "Ethane/Core/Base.h"

#include "Ethane/Core/KeyCodes.h"
#include "Ethane/Core/MouseCodes.h"

namespace Ethane {
	
	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}