#pragma once

#include "Ethane/Renderer/Shader.h"
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Ethane {

	class ShaderLibrary
	{
	public:
		static void Init();
		static void Shutdown();

		static void Add(const std::string& name, const Ref<Shader>& shader);
		static void Add(const Ref<Shader>& shader);
		static Ref<Shader> Load(const std::string& filepath);
		static Ref<Shader> Load(const std::string& name, const std::string& filepath);

		static Ref<Shader> Get(const std::string& name);

		static bool Exists(const std::string& name);
	private:
		static std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}