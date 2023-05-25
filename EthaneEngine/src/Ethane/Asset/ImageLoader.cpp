//
//  ImageLoader.cpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/22.
//

#include "ImageLoader.h"
#include "stb_image.h"

namespace Ethane {

bool ImageLoader::LoadImage(const std::string& filepath, ImageResource& out_resource)
{
    stbi_uc* data = nullptr;
    int width, height, channel;
    data = stbi_load(filepath.c_str(), &width, &height, &channel, STBI_rgb_alpha);
    
    out_resource.Width = width;
    out_resource.Height = height;
    out_resource.OriginalChannels = channel;
    out_resource.DataSize = width * height * 4;
    
    if (!data) {
        ETH_CORE_ASSERT("Failed to load image!");
        return false;
    }
    out_resource.Data = data;
    
    ETH_CORE_TRACE("w: {0}, h: {1}, ch: {2}", out_resource.Width, out_resource.Height, out_resource.OriginalChannels);
    return true;
}

void ImageLoader::UnloadImage(ImageResource& resource)
{
    stbi_image_free(resource.Data);
}

}
