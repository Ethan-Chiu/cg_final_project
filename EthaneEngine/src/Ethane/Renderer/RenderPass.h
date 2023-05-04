#pragma once

#include "Ethane/Core/Base.h"

namespace Ethane {
	
	class RenderPass 
	{
	public:
		virtual ~RenderPass() = default;

		static Ref<RenderPass> Create();
	};
}
