#pragma once

#include <glm/glm.hpp>

namespace Ethane {

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1, Vulkan = 2
		};

	public:
		virtual ~RendererAPI() = default;

		inline static API GetAPI() { return s_API; }
		inline static void SetAPI(API _API) { s_API = _API; }

	private:
		static API s_API;
	};

}