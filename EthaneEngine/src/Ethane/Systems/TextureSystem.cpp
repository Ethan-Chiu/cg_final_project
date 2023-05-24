//
//  TextureSystem.cpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/23.
//

#include "ethpch.h"
#include "TextureSystem.h"
#include "ResourceSystem.h"
#include "Ethane/Renderer/Renderer.h"

namespace Ethane {

    bool TextureSystem::Init()
    {
        ImageResource imageData;
        ResourceSystem::LoadImage("/Users/ethan/ethans_folder/Program_dev/cg_final_project/Ethane-Editor/assets/textures/test.png", imageData);
        ImageSpecification imageSpec;
        imageSpec.Width = imageData.Width;
        imageSpec.Height = imageData.Height;
        
        s_DefaultImage = Image2D::Create(imageSpec, imageData.Data);
        s_DefaultTexture = Texture2D::Create(s_DefaultImage.get());
        
        return true;
    }

    void TextureSystem::Shutdown()
    {
        s_DefaultTexture->Destroy();
        s_DefaultImage->Destroy();
    }

}
