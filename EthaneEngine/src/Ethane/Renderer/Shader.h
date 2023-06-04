#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

#include "GraphicsContext.h"

namespace Ethane {

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Destroy() = 0;

		virtual const std::string& GetName() const = 0;

		static Ref<Shader> Create(const std::string& filepath);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	};

}
