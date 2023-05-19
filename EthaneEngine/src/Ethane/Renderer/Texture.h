#pragma once

#include <string>
#include "Ethane/Core/Base.h"

#include "Image.h"

namespace Ethane {

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

//		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const GraphicsContext* ctx, Image2D* image);
	};
}
