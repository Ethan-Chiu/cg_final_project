#pragma once

#include "Ethane/Core/Base.h"
#include "GraphicsContext.h"

namespace Ethane {

	enum class ImageFormat
	{
		None = 0,
		RED32F,
		RGB,
		RGBA,
		RGBA16F,
		RGBA32F,
		RG16F,
		RG32F,
		SRGB,
        BGR,
        BGRA,

		DEPTH32F,
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8,
	};

	enum class ImageUsage
	{
		None = 0,
		Texture,
		Attachment,
		Storage
	};

	struct ImageSpecification
	{
		ImageFormat Format = ImageFormat::RGBA;
		ImageUsage Usage = ImageUsage::Texture;
		uint32_t Width = 1;
		uint32_t Height = 1;
		uint32_t Mips = 1;
		uint32_t Layers = 1;
		std::string DebugName;
	};

    enum class ImageLayout
    {
        Undefined,
        General,
        ShaderRead,
        PresentSRC,
    };

    enum class AccessMask
    {
        None,
        ShaderRead,
        ShaderWrite,
        ColorRead,
        ColorWrite,
        TransferRead,
        TransferWrite,
        MemoryRead,
        MemoryWrite,
    };

    enum class PipelineStage
    {
        None,
        PipeTop,
        VertexShader,
        FragmentShader,
        ComputeShader,
        Transfer,
        PipeBottom,
    };


	class Image
	{
	public:
		virtual ~Image() {}
        
        virtual void Destroy() = 0;
		// Getter
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual float GetAspectRatio() const = 0;
		virtual ImageSpecification& GetSpecification() = 0;
		virtual const ImageSpecification& GetSpecification() const = 0;
	};

	class Image2D : public Image
	{
	public:
		static Ref<Image2D> Create(ImageSpecification specification, void* buffer = nullptr, uint32_t size = 0);
	};

    class TargetImage : public Image
    {
    public:
        static Ref<TargetImage> Create(ImageSpecification specification);
    };

	namespace Utils {
		inline bool IsDepthFormat(ImageFormat format)
		{
			if (format == ImageFormat::DEPTH24STENCIL8 || format == ImageFormat::DEPTH32F)
				return true;

			return false;
		}
	}
}
