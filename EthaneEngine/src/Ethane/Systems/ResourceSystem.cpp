//
//  ResourceSystem.cpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/23.
//

#include "ethpch.h"
#include "ResourceSystem.h"

namespace Ethane {

    ImageLoader ResourceSystem::s_ImageLoader = ImageLoader();

    bool ResourceSystem::Init()
    {
        return true;
    }

    void ResourceSystem::Shutdown()
    {
        
    }
    
    void ResourceSystem::LoadImage(const std::string& filepath, ImageResource& out_resource)
    {
        s_ImageLoader.LoadImage(filepath, out_resource);
    }
}
