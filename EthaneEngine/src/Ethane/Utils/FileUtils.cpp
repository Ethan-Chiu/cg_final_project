//
//  FileUtils.cpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/6/3.
//

#include "ethpch.h"
#include "FileUtils.hpp"

namespace Ethane {

std::string FileUtils::ReadFile(const std::string& filepath)
{
    ETH_PROFILE_FUNCTION();

    std::string result;
    std::ifstream in(filepath, std::ios::in | std::ios::binary);
    if (in)
    {
        in.seekg(0, std::ios::end);
        result.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&result[0], result.size());
    }
    else
    {
        ETH_CORE_ERROR("Could not open file '{0}'", filepath);
    }
    in.close();
    return result;
}

}
