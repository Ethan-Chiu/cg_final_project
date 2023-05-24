//
//  ShaderSystem.hpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/23.
//

#pragma once

#include "Ethane/Renderer/Shader.h"

namespace Ethane {

class ShaderSystem {
    
public:
    static bool Init();
    
    static void Shutdown();
    
    static Ref<Shader> Load(const std::string& filepath);
    static Ref<Shader> Load(const std::string& name, const std::string& filepath);

    static Ref<Shader> Get(const std::string& name);
    static bool Exists(const std::string& name);
    
    static void SetUniformData();
    
private:
    inline static std::unordered_map<std::string, Ref<Shader>> s_Shaders = {};
};

}
