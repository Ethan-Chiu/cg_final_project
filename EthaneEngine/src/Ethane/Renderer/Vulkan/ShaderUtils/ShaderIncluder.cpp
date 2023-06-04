//
//  ShaderIncluder.cpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/6/3.
//

#include "ethpch.h"
#include "ShaderIncluder.hpp"
#include "Ethane/Renderer/Shader.h"
#include "Ethane/Utils/FileUtils.hpp"

namespace Ethane {

shaderc_include_result* ShaderIncluder::GetInclude(
    const char* requested_source,
    shaderc_include_type type,
    const char* requesting_source,
    size_t include_depth)
{
    const std::string name = std::string(requested_source);
    const std::string contents = FileUtils::ReadFile(name);
    
    ETH_CORE_INFO("including {0}", name);

    auto container = new std::array<std::string, 2>;
    (*container)[0] = name;
    (*container)[1] = contents;

    auto data = new shaderc_include_result;

    data->user_data = container;

    data->source_name = (*container)[0].data();
    data->source_name_length = (*container)[0].size();

    data->content = (*container)[1].data();
    data->content_length = (*container)[1].size();

    return data;
};

void ShaderIncluder::ReleaseInclude(shaderc_include_result* data)
{
    delete static_cast<std::array<std::string, 2>*>(data->user_data);
    delete data;
};

}
