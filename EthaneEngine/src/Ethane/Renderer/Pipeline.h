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
		const RenderPass* RenderPass;
		VertexBufferLayout Layout;
	};

    struct ComputePipelineSpecification
    {
        Ref<Shader> Shader;
    };

	class Pipeline
	{
	public:
		virtual ~Pipeline() = default;

		virtual PipelineSpecification& GetSpecification() = 0;
		virtual const PipelineSpecification& GetSpecification() const = 0;

        virtual void Destroy() = 0;

		static Ref<Pipeline> Create(const PipelineSpecification& spec);
	};

    class ComputePipeline
    {
    public:
        virtual ~ComputePipeline() = default;

        virtual ComputePipelineSpecification& GetSpecification() = 0;
        virtual const ComputePipelineSpecification& GetSpecification() const = 0;

        virtual void Destroy() = 0;

        static Ref<ComputePipeline> Create(const ComputePipelineSpecification& spec);
    };
}
