//
//  TextureSystem.hpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/23.
//
#pragma once

#include "Ethane/Renderer/Image.h"
#include "Ethane/Renderer/Texture.h"

namespace Ethane {

struct TextureImage
{
    Ref<Image2D> AssociatedImage;
    Ref<Texture2D> Texture;
};

class TextureSystem {
public:
    static bool Init();
    
    static void Shutdown();
    
    static Texture2D* GetTexture(const std::string& path, ImageUsage imageUsage);
    
    static const Ref<Texture> GetDefaultTexture() { return s_DefaultTexture; };
    
private:
    inline static Ref<Image2D> s_DefaultImage = nullptr;
    inline static Ref<Texture2D> s_DefaultTexture = nullptr;
    
    inline static std::unordered_map<std::string, TextureImage> s_LoadedTexture;
};
}
