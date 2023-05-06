#pragma once

#include "Ethane/Core/Base.h"

#include "GraphicsContext.h"

#include "Shader.h"
#include "RenderPass.h"
#include "VertexBuffer.h"

namespace Ethane {

	struct PipelineSpecification
	{
		Ref<Shader> Shader;
		Ref<RenderPass> RenderPass;
		VertexBufferLayout Layout;
	};

	class Pipeline
	{
	public:
		virtual ~Pipeline() = default;

		virtual PipelineSpecification& GetSpecification() = 0;
		virtual const PipelineSpecification& GetSpecification() const = 0;

		virtual void Create() = 0;

		static Ref<Pipeline> Create(const GraphicsContext* ctx, const PipelineSpecification& spec);
	};
}