#pragma once

#include "GraphicsContext.h"

namespace Ethane {

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void SetData(void* buffer, uint32_t size, uint32_t offset = 0) = 0;
        
		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(const GraphicsContext* ctx, uint32_t* indices, uint32_t count);
	};

}
