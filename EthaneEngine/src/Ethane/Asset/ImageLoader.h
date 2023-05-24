//
//  ImageLoader.hpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/22.
//
#pragma once

namespace Ethane {

struct ImageResource {
    uint32_t Width;
    uint32_t Height;
    uint32_t Channels;
    void* Data;
};

class ImageLoader
{
public:
    bool LoadImage(const std::string& filepath, ImageResource& out_resource);
    
    void UnloadImage(ImageResource& resource);
};
}
