#include "ethpch.h"
#include "ShaderLibrary.h"

namespace Ethane {

	std::unordered_map<std::string, Ref<Shader>> ShaderLibrary::m_Shaders = {};

	void ShaderLibrary::Init()
	{
		Load("assets/shaders/Texture.glsl");
		Load("assets/shaders/PBR_static.glsl");
		Load("assets/shaders/SceneComposite.glsl");
		Load("assets/shaders/Grid.glsl");
		// Load("assets/shaders/test.glsl");
	}

	void ShaderLibrary::Shutdown()
	{
		m_Shaders.clear();
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		ETH_CORE_ASSERT(!Exists(name), "Shader already exist!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
//		auto shader = Shader::Create(filepath);
//		Add(shader);
//		return shader;
        return nullptr; // TODO
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
//		auto shader = Shader::Create(filepath);
//		Add(name, shader);
//		return shader;
        return nullptr; // TODO
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		ETH_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name)
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
}
