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

		//Uniform
		virtual uint32_t GetUniformBufferIndex(uint32_t bindingPoint) = 0;
		virtual void SetUniformBuffer(uint32_t uboIndex, const void* data, uint32_t size, uint32_t offset = 0) = 0;
		virtual void SetUniformBufferByBindingPoint(uint32_t bindingPoint, const void* data, uint32_t size, uint32_t offset = 0) = 0;
		virtual uint32_t GetUniformBufferIndex(const std::string& name) = 0; // test
		virtual void SetUniformBufferByName(const std::string& name, const void* data, uint32_t size) =  0; // test


		virtual const std::string& GetName() const = 0;

		std::string ReadFile(const std::string& filepath);

		static Ref<Shader> Create(const GraphicsContext* ctx, const std::string& filepath);
		static Ref<Shader> Create(const GraphicsContext* ctx, const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	};

}
