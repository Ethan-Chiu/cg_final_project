//
//  RenderTarget.hpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/20.
//

#pragma once

#include "Image.h"
#include "RenderPass.h"

namespace Ethane {

struct RenderTargetImageSpecification
{
    RenderTargetImageSpecification() = default;
    RenderTargetImageSpecification(Image* image)
        : AttachImage(image) {}

    Image* AttachImage;
    // TODO: filtering/wrap
};

struct RenderTargetAttachmentSpecification
{
    RenderTargetAttachmentSpecification() = default;
    RenderTargetAttachmentSpecification(std::initializer_list<RenderTargetImageSpecification> attachments)
        : Attachments(attachments) {}

    std::vector<RenderTargetImageSpecification> Attachments;
};

struct RenderTargetSpecification
{
    std::string DebugName = "RenderTarget";
    
    uint32_t Width, Height;
    glm::vec4 ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    
    bool IsTargetImage = false;
    RenderTargetAttachmentSpecification Attachments;
    
    bool SwapChainTarget = false;
    bool ClearOnLoad = true;
    bool Blend = true;
};

class RenderTarget{
public:
    virtual ~RenderTarget() = default;

    virtual void Resize(uint32_t width, uint32_t height) = 0;
    virtual void Destroy() = 0;
    
    // Getter
    virtual const RenderTargetSpecification& GetSpecification() const = 0;
    virtual const RenderPass* GetRenderPass() const = 0;

    static Scope<RenderTarget> Create(const RenderTargetSpecification& spec);
};
}
