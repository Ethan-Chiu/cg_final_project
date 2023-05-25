//
//  ShaderSystem.cpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/23.
//

#include "ShaderSystem.h"
#include "Ethane/Renderer/Renderer.h"

namespace Ethane {

bool ShaderSystem::Init()
{
    return true;
}

void ShaderSystem::Shutdown()
{
    for(auto [name, shader]: s_Shaders)
    {
        shader->Destroy();
    }
    s_Shaders.clear();
}

Ref<Shader> ShaderSystem::Load(const std::string& filepath)
{
    auto lastSlash = filepath.find_last_of("/\\");
    lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
    auto lastDot = filepath.rfind('.');
    auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
    std::string name = filepath.substr(lastSlash, count);
    
    return Load(name, filepath);
}

Ref<Shader> ShaderSystem::Load(const std::string& name, const std::string& filepath)
{
    if (Exists(name))
    {
        ETH_CORE_WARN("Shader already exist!");
        return s_Shaders[name];
    }
    
    auto shader = Shader::Create(filepath);
    
    Renderer::RegisterShader(shader.get());
    
    s_Shaders[name] = shader;
    return s_Shaders[name];
}

Ref<Shader> ShaderSystem::Get(const std::string& name)
{
    ETH_CORE_ASSERT(Exists(name), "Shader not found!");
    return s_Shaders[name];
}

bool ShaderSystem::Exists(const std::string& name)
{
    return s_Shaders.find(name) != s_Shaders.end();
}


void ShaderSystem::SetUniformData()
{
    
}

}
