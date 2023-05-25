#include "ethpch.h"
#include "Ethane/Core/timer.h"

#define GLM_FORECE_DEPT_ZERO_TO_ONE // TODO: move
#include <GLFW/glfw3.h>

#include "VulkanSwapChain.h"
#include "VulkanRenderPass.h"
#include "VulkanTargetImage.h"


namespace Ethane {

    VulkanSwapChain::~VulkanSwapChain()
    {
        ETH_CORE_TRACE("VulkanSwapChain destructed");
    }

    VulkanSwapChain::VulkanSwapChain(VkSurfaceKHR surface, VulkanDevice* device, uint32_t width, uint32_t height, bool vsync)
        : m_Surface(surface),
        m_Device(device),
        m_PhysicalDevice(device->GetPhysicalDevice()),
        m_VSync(vsync),
        m_Width(width),
        m_Height(height)
    {
        m_TargetImage = nullptr;
    }

    Scope<VulkanSwapChain> VulkanSwapChain::Create(VkSurfaceKHR surface, VulkanDevice* device, uint32_t width, uint32_t height, bool vsync)
	{
		return CreateScope<VulkanSwapChain>(surface, device, width, height, vsync);
	}

    void VulkanSwapChain::Init()
    {
        // for profiling
        Timer timer;

        VkSwapchainKHR oldSwapchain = m_SwapChain;

        VkDevice device = m_Device->GetVulkanDevice();
        VkPhysicalDevice physicalDevice = m_PhysicalDevice->GetVulkanPhysicalDevice();


        SwapChainSupportDetails swapChainSupport = m_PhysicalDevice->QuerySwapChainSupport(physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
        m_Extent = ChooseSwapExtent(swapChainSupport.capabilities);
        ETH_CORE_INFO("width: {0}, height: {1}", m_Extent.width, m_Extent.height);

        m_ImageFormat = surfaceFormat.format;
        m_ImageColorSpace = surfaceFormat.colorSpace;
        ETH_CORE_INFO("Swapchain imgae Format: {0} | Image Color Space: {1}", m_ImageFormat, m_ImageColorSpace);

        // profiling
        ETH_CORE_TRACE("choose time: {0}ms", timer.ElapsedMillis());
        timer.Reset();

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        m_MaxFramesInFlight = imageCount - 1;
        ETH_CORE_TRACE("Max frames in flight is {0}", m_MaxFramesInFlight);


        // Find the transformation of the surface
        VkSurfaceTransformFlagsKHR preTransform;
        if (swapChainSupport.capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
        {
            // We prefer a non-rotated transform
            preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        }
        else
        {
            preTransform = swapChainSupport.capabilities.currentTransform;
        }

        // test
        // // Find a supported composite alpha format (not all devices support alpha opaque)
        // VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        // // Simply select the first composite alpha format available
        // std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
        //     VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        //     VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        //     VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        //     VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
        // };
        // for (auto& compositeAlphaFlag : compositeAlphaFlags) {
        //     if (surfCaps.supportedCompositeAlpha & compositeAlphaFlag) {
        //         compositeAlpha = compositeAlphaFlag;
        //         break;
        //     };
        // }

        VkSwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.pNext = nullptr;
        swapchainCreateInfo.surface = m_Surface;
        swapchainCreateInfo.minImageCount = imageCount;
        swapchainCreateInfo.imageFormat = m_ImageFormat;
        swapchainCreateInfo.imageColorSpace = m_ImageColorSpace;
        swapchainCreateInfo.imageExtent = m_Extent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        const auto& indices = m_PhysicalDevice->GetQueueFamilyIndices();
        if (indices.Graphics.value() != indices.Present.value()) {
            uint32_t queueFamilyIndices[] = { indices.Graphics.value(), indices.Present.value() };
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainCreateInfo.queueFamilyIndexCount = 2;
            swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreateInfo.queueFamilyIndexCount = 0;
            swapchainCreateInfo.pQueueFamilyIndices = nullptr;
        }
        swapchainCreateInfo.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = presentMode;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.oldSwapchain = oldSwapchain;

        VK_CHECK_RESULT(vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &m_SwapChain));

        // test
        // destroy the old swap chain, if exist
        // This also cleans up all the presentable images
        if (oldSwapchain != VK_NULL_HANDLE)
        {
            ETH_CORE_WARN("Clean up old swapchain");
            CleanupSwapChain(oldSwapchain);
        }

        // Get the swap chain images
        VK_CHECK_RESULT(vkGetSwapchainImagesKHR(device, m_SwapChain, &m_ImageCount, nullptr));
        m_Images.resize(m_ImageCount);
        VK_CHECK_RESULT(vkGetSwapchainImagesKHR(device, m_SwapChain, &m_ImageCount, m_Images.data()));

        std::vector<ImageUtils::VulkanImageInfo> imageInfos;
        // create imageview
        m_ImageViews.resize(m_ImageCount);
        for (uint32_t i = 0; i < m_ImageCount; i++)
        {
            VkImageViewCreateInfo colorAttachmentView = {};
            colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            colorAttachmentView.pNext = nullptr;
            colorAttachmentView.image = m_Images[i];
            colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
            colorAttachmentView.format = m_ImageFormat;
            colorAttachmentView.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            colorAttachmentView.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            colorAttachmentView.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            colorAttachmentView.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            colorAttachmentView.subresourceRange.baseMipLevel = 0;
            colorAttachmentView.subresourceRange.levelCount = 1;
            colorAttachmentView.subresourceRange.baseArrayLayer = 0;
            colorAttachmentView.subresourceRange.layerCount = 1;
            colorAttachmentView.flags = 0;

            VK_CHECK_RESULT(vkCreateImageView(device, &colorAttachmentView, nullptr, &m_ImageViews[i]));
            imageInfos.push_back(ImageUtils::VulkanImageInfo{m_Images[i], m_ImageViews[i]});
        }
        
        ImageSpecification imageSpec = {};
        imageSpec.Format = ImageUtils::VulkanImageFormatToImageFormat(m_ImageFormat);
        imageSpec.Usage = ImageUsage::Attachment;
        imageSpec.Width = m_Width;
        imageSpec.Height = m_Height;
        imageSpec.Mips = 1;
        imageSpec.Layers = 1;
        imageSpec.DebugName = "swapchain image";
        
        if(m_TargetImage == nullptr) {
            m_TargetImage = CreateScope<VulkanTargetImage>(imageSpec, imageInfos);
        } else {
            m_TargetImage->SwapchainUpdate(imageSpec, imageInfos);
        }

        // Synchronization Objects
        if (m_ImageAvailableSemaphores.empty() || m_RenderFinishedSemaphores.empty() || m_InFlightFences.empty())
        {
            ETH_CORE_TRACE("Create Synchronization Objects");
            m_ImageAvailableSemaphores.resize(m_MaxFramesInFlight);
            m_RenderFinishedSemaphores.resize(m_MaxFramesInFlight);
            m_InFlightFences.resize(m_MaxFramesInFlight);
            m_ImagesInFlight.resize(m_Images.size(), VK_NULL_HANDLE);

            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkFenceCreateInfo fenceInfo{};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            for (size_t i = 0; i < m_MaxFramesInFlight; i++) {
                VK_CHECK_RESULT(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]));
                VK_CHECK_RESULT(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]));
                VK_CHECK_RESULT(vkCreateFence(device, &fenceInfo, nullptr, &m_InFlightFences[i]));
            }
        }

        // profiling
        ETH_CORE_TRACE("create time: {0}ms", timer.ElapsedMillis());
        timer.Reset();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        CreateCommandBuffers();

        ETH_CORE_INFO("SwapChain created");
    }

    // private func
    VkSurfaceFormatKHR VulkanSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR VulkanSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        }
        else {
            int width, height;
            width = m_Width;
            height = m_Height;
            // glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    void VulkanSwapChain::CreateCommandBuffers()
    {
        if (m_GraphicsCommandBuffers.empty()) {
            m_GraphicsCommandBuffers.resize(m_MaxFramesInFlight, VulkanCommandBuffer(m_Device));
        }

        for (uint32_t i = 0; i < m_MaxFramesInFlight; ++i) {
            if (m_GraphicsCommandBuffers[i].GetHandle()) {
                m_GraphicsCommandBuffers[i].Free(m_Device->GetGraphicsCommandPool());
            }
            m_GraphicsCommandBuffers[i].Allocate(m_Device->GetGraphicsCommandPool(), true);
        }

        ETH_CORE_INFO("Vulkan command buffers created.");
    }

    void VulkanSwapChain::OnResize(uint32_t width, uint32_t height)
    {
        // NOTE: width == 0 or height == 0 will not occur
        m_Width = width;
        m_Height = height;
        m_NeedResize = true;
    }

    bool VulkanSwapChain::Resize()
    {
        ETH_CORE_WARN("VulkanSwapChain::Resize");
        
        if (m_IsRecreating) {
            ETH_CORE_WARN("Already recreating swapchain");
            return false;
        }

        m_IsRecreating = true;

        auto device = m_Device->GetVulkanDevice();
        vkDeviceWaitIdle(device);

        Init();

        m_NeedResize = false;
        m_IsRecreating = false;
        return true;
    }

    bool VulkanSwapChain::BeginFrame()
    {
        auto device = m_Device->GetVulkanDevice();

        if(m_IsRecreating) {
            VK_CHECK_RESULT(vkDeviceWaitIdle(device));
            return false;
        }

        if (m_NeedResize) {
            VK_CHECK_RESULT(vkDeviceWaitIdle(device));
            Resize();
            return false;
        }

        // TODO: check result
        vkWaitForFences(device, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);
        VK_CHECK_RESULT(vkResetFences(device, 1, &m_InFlightFences[m_CurrentFrame]));


        if (!AcquireNextImage()) {
            return false;
        }

        VulkanCommandBuffer currentCommandBuffer = m_GraphicsCommandBuffers[m_CurrentFrame];
        currentCommandBuffer.Reset();
        currentCommandBuffer.Begin(false, false, false);

//        SetViewportAndScissor();

        return true;
    }

    void VulkanSwapChain::SetViewportAndScissor()
    {
        VulkanCommandBuffer currentCommandBuffer = m_GraphicsCommandBuffers[m_CurrentFrame];

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)m_Extent.width;
        viewport.height = (float)m_Extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(currentCommandBuffer.GetHandle(), 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = m_Extent;
        vkCmdSetScissor(currentCommandBuffer.GetHandle(), 0, 1, &scissor);
    }

//    void VulkanSwapChain::BeginRenderPass()
//    {
//        VulkanCommandBuffer currentCommandBuffer = m_GraphicsCommandBuffers[m_CurrentFrame];
//
//        std::array<VkClearValue, 1> clearValues{};
//        clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
//#if depth
//        clearValues[1].depthStencil = { 1.0f, 0 };
//#endif
//
//        VkRenderPassBeginInfo renderPassInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
//        renderPassInfo.renderPass = m_RenderPass->GetHandle();
//        renderPassInfo.framebuffer = m_Framebuffers[m_CurrentImageIndex];
//        renderPassInfo.renderArea.offset = { 0, 0 };
//        renderPassInfo.renderArea.extent = m_Extent;
//        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());;
//        renderPassInfo.pClearValues = clearValues.data();
//        vkCmdBeginRenderPass(currentCommandBuffer.GetHandle(), &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
//    }

    void VulkanSwapChain::RegisterSecondaryCmdBuffer(VkCommandBuffer secondaryBuffer)
    {
        m_SecondaryCommandBuffers.push_back(secondaryBuffer);
    }

    void VulkanSwapChain::EndFrame()
    {
        auto device = m_Device->GetVulkanDevice();
        VulkanCommandBuffer currentCommandBuffer = m_GraphicsCommandBuffers[m_CurrentFrame];
        VkCommandBuffer currentCmdBufferHandle = currentCommandBuffer.GetHandle();

        if (uint32_t(m_SecondaryCommandBuffers.size()) > 0)
        {
            vkCmdExecuteCommands(currentCmdBufferHandle, uint32_t(m_SecondaryCommandBuffers.size()), m_SecondaryCommandBuffers.data());
            m_SecondaryCommandBuffers.clear();
        }

//        vkCmdEndRenderPass(currentCmdBufferHandle);
        VK_CHECK_RESULT(vkEndCommandBuffer(currentCmdBufferHandle));


        VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };

        VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;

        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.pWaitDstStageMask = waitStages;

        VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &currentCmdBufferHandle;

        VK_CHECK_RESULT(vkQueueSubmit(m_Device->GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_CurrentFrame]));

        Present(m_Device->GetGraphicsQueue(), m_RenderFinishedSemaphores[m_CurrentFrame]);

        m_CurrentFrame = (++m_CurrentFrame) % m_MaxFramesInFlight;
    }


    bool VulkanSwapChain::AcquireNextImage()
    {
        auto device = m_Device->GetVulkanDevice(); // TODO: save device as member

        VkResult result = vkAcquireNextImageKHR(device, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &m_CurrentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            Resize();
            return false;
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            ETH_CORE_ERROR("Swapchain require next image failed");
            return false;
        }
        return true;
    }

    void VulkanSwapChain::Present(VkQueue queue, VkSemaphore signalSemaphore)
    {
        ETH_PROFILE_FUNCTION();

        VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        presentInfo.pNext = nullptr;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_SwapChain;
        presentInfo.pImageIndices = &m_CurrentImageIndex;
        // Check if a wait semaphore has been specified to wait for before presenting the image
        if (signalSemaphore != nullptr)
        {
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = &signalSemaphore;
        }
        else
        {
            ETH_CORE_WARN("Signal semaphores for queue present is not provided!");
        }

        VkResult result = vkQueuePresentKHR(queue, &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            Resize();
        }
        else if(result != VK_SUCCESS)
        {
            ETH_CORE_ASSERT("Queue present error");
        }
    }

    void VulkanSwapChain::CleanupSwapChain(VkSwapchainKHR swapchain) {
        VkDevice device = m_Device->GetVulkanDevice();

        vkDeviceWaitIdle(device);

//        for (auto framebuffer : m_Framebuffers)
//        {
//            vkDestroyFramebuffer(device, framebuffer, nullptr);
//        }
//
//        m_RenderPass->Destroy();

//        m_DepthAttachment->Destroy();

        for (auto imageView : m_ImageViews)
        {
            vkDestroyImageView(device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(device, swapchain, nullptr);
    }

    void VulkanSwapChain::Destroy() {
        VkDevice device = m_Device->GetVulkanDevice();

        vkDeviceWaitIdle(device);

        ETH_CORE_INFO("Destroying Vulkan command buffers...");
        for (auto& commandBuffer : m_GraphicsCommandBuffers) {
            if (commandBuffer.GetHandle()) {
                commandBuffer.Free(m_Device->GetGraphicsCommandPool());
            }
        }
        m_GraphicsCommandBuffers.clear();

        ETH_CORE_INFO("Destroying Vulkan swapchain sync objects...");
        for (size_t i = 0; i < m_MaxFramesInFlight; i++) {
            vkDestroySemaphore(device, m_RenderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device, m_ImageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device, m_InFlightFences[i], nullptr);
        }

        m_TargetImage = nullptr;
        CleanupSwapChain(m_SwapChain);
    }

}
