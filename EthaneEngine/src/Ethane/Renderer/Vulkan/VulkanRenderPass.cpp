#include "ethpch.h"
#include "VulkanRenderPass.h"
#include "VulkanContext.h"

namespace Ethane {

    Scope<VulkanRenderPass> VulkanRenderPass::Create(VkDevice device, const std::vector<ImageFormat>& colorAttachmentFormats, ImageFormat depthAttachmentFormat, bool clearOnLoad, bool finalPresent)
    {
        return CreateScope<VulkanRenderPass>(device, colorAttachmentFormats, depthAttachmentFormat, clearOnLoad, finalPresent);
    }

    VulkanRenderPass::VulkanRenderPass(VkDevice device, const std::vector<ImageFormat>& colorAttachmentFormats, ImageFormat depthAttachmentFormat, bool clearOnLoad, bool finalPresent)
    {
        m_Device = device;
        
        std::vector<VkAttachmentDescription> attachmentDescriptions;
        std::vector<VkAttachmentReference> colorAttachmentReferences;
        VkAttachmentReference depthAttachmentReference;

        uint32_t attachmentIndex = 0;
        bool hasDepth = (depthAttachmentFormat != ImageFormat::None);
        
        for (auto attachmentFormat : colorAttachmentFormats)
        {
            VkAttachmentDescription& attachmentDescription = attachmentDescriptions.emplace_back();
            attachmentDescription.flags = 0;
            attachmentDescription.format = ImageUtils::VulkanImageFormat(attachmentFormat);
            attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
            attachmentDescription.loadOp = clearOnLoad ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
            attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // TODO: if sampling, needs to be store (otherwise DONT_CARE is fine)
            attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachmentDescription.initialLayout = clearOnLoad ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            attachmentDescription.finalLayout = finalPresent ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            
            colorAttachmentReferences.emplace_back(VkAttachmentReference{ attachmentIndex, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });

            attachmentIndex++;
        }
        
        if (hasDepth)
        {
            VkAttachmentDescription& attachmentDescription = attachmentDescriptions.emplace_back();
            attachmentDescription.flags = 0;
            attachmentDescription.format = ImageUtils::VulkanImageFormat(depthAttachmentFormat);
            attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
            attachmentDescription.loadOp = clearOnLoad ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
            attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // TODO: if sampling, needs to be store (otherwise DONT_CARE is fine)
            attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachmentDescription.initialLayout = clearOnLoad ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            if (depthAttachmentFormat == ImageFormat::DEPTH24STENCIL8 || true) // Separate layouts requires a "separate layouts" flag to be enabled
            {
                attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; // TODO: if not sampling
                attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL; // TODO: if sampling
                depthAttachmentReference = { attachmentIndex, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
            }
            else
            {
                attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL; // TODO: if not sampling
                attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL; // TODO: if sampling
                depthAttachmentReference = { attachmentIndex, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL };
            }
            attachmentIndex++;
        }

        std::vector<VkSubpassDescription> subpasses;
        std::vector<VkSubpassDependency>  subpassDependencies;
        
        
        VkSubpassDescription subpassDescription = {};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.colorAttachmentCount = uint32_t(colorAttachmentReferences.size());
        subpassDescription.pColorAttachments = colorAttachmentReferences.data();
        if (hasDepth)
            subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
        subpassDescription.inputAttachmentCount = 0;
        subpassDescription.pInputAttachments = VK_NULL_HANDLE;
        subpassDescription.pResolveAttachments = VK_NULL_HANDLE;
        subpassDescription.preserveAttachmentCount = 0;
        subpassDescription.pPreserveAttachments = VK_NULL_HANDLE;
        
        subpasses.push_back(subpassDescription);


        if (colorAttachmentFormats.size())
        {
            {
                VkSubpassDependency& dependency = subpassDependencies.emplace_back();
                dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // TODO: i == 0 ? : (i - 1);
                dependency.dstSubpass = 0; // i
                dependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                dependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;  // VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
                // depedency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

                // TODO: test
                dependency.dstSubpass = 0;
                dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependency.srcAccessMask = 0;
                dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            }
            {
                VkSubpassDependency& dependency = subpassDependencies.emplace_back();
                dependency.srcSubpass = 0;
                dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
                dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            }
        }

        if (hasDepth)
        {
            {
                VkSubpassDependency& depedency = subpassDependencies.emplace_back();
                depedency.srcSubpass = VK_SUBPASS_EXTERNAL;
                depedency.dstSubpass = 0;
                depedency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                depedency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                depedency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                depedency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                depedency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            }

            {
                VkSubpassDependency& depedency = subpassDependencies.emplace_back();
                depedency.srcSubpass = 0;
                depedency.dstSubpass = VK_SUBPASS_EXTERNAL;
                depedency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                depedency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                depedency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                depedency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                depedency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            }
        }

        // Create the actual renderpass
        VkRenderPassCreateInfo renderPassInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
        renderPassInfo.pNext = nullptr;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
        renderPassInfo.pAttachments = attachmentDescriptions.data();
        renderPassInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
        renderPassInfo.pSubpasses = subpasses.data();
        renderPassInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
        renderPassInfo.pDependencies = subpassDependencies.data();
        renderPassInfo.flags = 0;
        
        VK_CHECK_RESULT(vkCreateRenderPass(m_Device, &renderPassInfo, nullptr, &m_RenderPass));
    }

    void VulkanRenderPass::Destroy()
    {
        if (m_RenderPass) {
            vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
        }
    }

    void VulkanRenderPass::Begin(VkCommandBuffer cmdBuffer, uint32_t width, uint32_t height, VkFramebuffer frameBuffer)
    {
        VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
        renderPassBeginInfo.pNext = nullptr;
        renderPassBeginInfo.renderPass = m_RenderPass;
        renderPassBeginInfo.renderArea.offset.x = 0;
        renderPassBeginInfo.renderArea.offset.y = 0;
        renderPassBeginInfo.renderArea.extent.width = width;
        renderPassBeginInfo.renderArea.extent.height = height;
        renderPassBeginInfo.framebuffer = frameBuffer;

        std::vector<VkClearValue> clearValues;
        VkClearValue& clearValue0 = clearValues.emplace_back();
        clearValue0.color.float32[0] = 0.0f;
        clearValue0.color.float32[1] = 0.0f;
        clearValue0.color.float32[2] = 0.0f;
        clearValue0.color.float32[3] = 1.0f;
        VkClearValue& clearValue1 = clearValues.emplace_back();
        clearValue1.depthStencil.depth = 1.0f;
        clearValue1.depthStencil.stencil = 0;

        renderPassBeginInfo.clearValueCount = (uint32_t)clearValues.size();
        renderPassBeginInfo.pClearValues = clearValues.data();
        
        vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        // cmdBuffer state change
    }

    void VulkanRenderPass::End(VkCommandBuffer cmdBuffer)
    {
        vkCmdEndRenderPass(cmdBuffer);
        // cmdBuffer state change
    }
}
