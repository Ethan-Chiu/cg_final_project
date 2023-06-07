#pragma once

#include "Ethane/Core/Base.h"

namespace Ethane {

	class RenderCommandBuffer
	{
	public:
		virtual ~RenderCommandBuffer() {}

		virtual void Begin() = 0;
		virtual void End() = 0;
        virtual void Wait() = 0;
		virtual void Submit() = 0;
        virtual void Destroy() = 0;

		static Ref<RenderCommandBuffer> Create(bool isInFlight, const std::string& debugName = "");
	};

}
