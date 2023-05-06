#pragma once

namespace Ethane {

	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual void Init() = 0;

		virtual bool BeginFrame() = 0;
		virtual void SwapBuffers() = 0;

		virtual void OnResize(uint32_t width, uint32_t height) = 0;

		virtual void Shutdown() = 0;

		static Scope<GraphicsContext> Create(void* window);
	};
}
