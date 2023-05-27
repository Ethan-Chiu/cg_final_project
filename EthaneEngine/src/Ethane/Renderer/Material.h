#pragma once
#include "Ethane/Core/Base.h"

#include "Shader.h"
#include "Texture.h"

namespace Ethane {

	enum class MaterialFlag
	{
		None = BIT(0),
		DepthTest = BIT(1),
		Blend = BIT(2),
		TwoSided = BIT(3)
	};

	class Material
	{
	public:
		static Ref<Material> Create(const Shader* shader, const std::string& name = "");

		virtual ~Material() {}
        virtual void Destroy() = 0;

		virtual void Invalidate() = 0;

		virtual const std::string& GetName() const = 0;
        virtual uint32_t GetFlags() const = 0;

        virtual bool SetImage(const std::string& name, const TargetImage* image) = 0;
        
        virtual const Shader* GetShader() const = 0;
	private:
		const Shader* m_Shader;
	};

}
