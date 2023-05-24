//
//  ResourceSystem.hpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/23.
//

#pragma once

#include "Ethane/Asset/ImageLoader.h"

namespace Ethane {
class ResourceSystem
{
public:
    static bool Init();
    
    static void Shutdown();
    
    static void LoadImage(const std::string& filepath, ImageResource& out_resource);
    
    static void UnloadImage(ImageResource& resource);
    
    static ImageLoader s_ImageLoader;
};
}
