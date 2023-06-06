//
//  TextureSystem.cpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/23.
//

#include "ethpch.h"
#include "Ethane/Asset/AssetManager.h"
#include "TextureSystem.h"
#include "ResourceSystem.h"
#include "Ethane/Renderer/Renderer.h"

namespace Ethane {

    bool TextureSystem::Init()
    {
        ImageResource imageData;
        ResourceSystem::LoadImage(AssetManager::GetBaseDirPath() + "cg_final_project/Ethane-Editor/assets/textures/test.png", imageData);
        ImageSpecification imageSpec;
        imageSpec.Format = ImageFormat::RGBA;
        imageSpec.Width = imageData.Width;
        imageSpec.Height = imageData.Height;
        
        s_DefaultImage = Image2D::Create(imageSpec, imageData.Data, imageData.DataSize);
        s_DefaultTexture = Texture2D::Create(s_DefaultImage.get());
        
        ResourceSystem::UnloadImage(imageData);
        
        return true;
    }

    void TextureSystem::Shutdown()
    {
        s_DefaultTexture->Destroy();
        s_DefaultImage->Destroy();
        
        for(auto&& [path, tex_image] : s_LoadedTexture)
        {
            tex_image.Texture->Destroy();
            tex_image.AssociatedImage->Destroy();
        }
        s_LoadedTexture.clear();
    }

    Texture2D* TextureSystem::GetTexture(const std::string& path, ImageUsage imageUsage)
    {
        if (s_LoadedTexture.find(path) != s_LoadedTexture.end())
        {
            if (s_LoadedTexture[path].AssociatedImage->GetSpecification().Usage == imageUsage)
                return s_LoadedTexture[path].Texture.get();
        }
            
        ImageResource imageData;
        ResourceSystem::LoadImage(path, imageData);
        ImageSpecification imageSpec;
        imageSpec.Usage = imageUsage;
        imageSpec.Format = ImageFormat::RGBA;
        imageSpec.Width = imageData.Width;
        imageSpec.Height = imageData.Height;
        
        s_LoadedTexture[path].AssociatedImage = Image2D::Create(imageSpec, imageData.Data, imageData.DataSize);
        s_LoadedTexture[path].Texture = Texture2D::Create(s_LoadedTexture[path].AssociatedImage.get());
        
        ResourceSystem::UnloadImage(imageData);
        return s_LoadedTexture[path].Texture.get();
    }

}
