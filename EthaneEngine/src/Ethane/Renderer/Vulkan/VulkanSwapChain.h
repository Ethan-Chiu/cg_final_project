#pragma once

#include "Vulkan.h"
#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"
#include "VulkanRenderPass.h"

struct GLFWwindow;

namespace Ethane{
    class VulkanTargetImage;

	class VulkanSwapChain
	{
	public:
		VulkanSwapChain() = default;
		~VulkanSwapChain();

		VulkanSwapChain(VkSurfaceKHR surface, VulkanDevice* device, bool vsync);
        static Scope<VulkanSwapChain> Create(VkSurfaceKHR surface, VulkanDevice* device, bool vsync);
		
		void Init();
		void Destroy();

		void CleanupSwapChain(VkSwapchainKHR swapchain);

		void OnResize(uint32_t width, uint32_t height);

		bool BeginFrame();
		void RegisterSecondaryCmdBuffer(VkCommandBuffer secondaryCmdBuffer);
		void EndFrame();

		// Getter
		VkSurfaceKHR GetSurface() const { return m_Surface; }
		VkFormat GetImageFormat() const { return m_ImageFormat; }
		uint32_t GetImageCount() const { return m_ImageCount; }
		uint32_t GetWidth() const { return m_Extent.width; }
		uint32_t GetHeight() const { return m_Extent.height; }
		uint32_t GetCurrentFrameIndex() const { return m_CurrentFrame; }
        uint32_t GetCurrentImageIndex() const { return m_CurrentImageIndex; }
		uint32_t GetMaxFramesInFlight() const { return m_MaxFramesInFlight; }
        VulkanTargetImage* GetTargetImage() const { return m_TargetImage.get(); }
        const VulkanCommandBuffer* GetCurrentCommandBuffer() const { return &m_GraphicsCommandBuffers[m_CurrentFrame]; }
        const VulkanCommandBuffer* GetCurrentComputeCommandBuffer() const { return &m_GraphicsCommandBuffers[m_CurrentFrame]; }

	private:
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
//		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		void CreateCommandBuffers();
        void CreateComputeCommandBuffers();

		bool Resize();
		bool AcquireNextImage();
		void Present(VkQueue queue, VkSemaphore signalSemaphore = VK_NULL_HANDLE);
	private:
		VkSwapchainKHR m_SwapChain = nullptr;

		const VulkanDevice* m_Device = nullptr;
		const VulkanPhysicalDevice* m_PhysicalDevice = nullptr;

		VkSurfaceKHR m_Surface;
		uint32_t m_Width = 0, m_Height = 0;
		VkExtent2D m_Extent;

		VkFormat m_ImageFormat;
		VkColorSpaceKHR m_ImageColorSpace;

		uint32_t m_ImageCount = 0;
		std::vector<VkImage> m_Images;
		std::vector<VkImageView> m_ImageViews;
        
        Scope<VulkanTargetImage> m_TargetImage;
        
		std::vector<VulkanCommandBuffer> m_GraphicsCommandBuffers;
        std::vector<VulkanCommandBuffer> m_ComputeCommandBuffers;
		std::vector<VkCommandBuffer> m_SecondaryCommandBuffers;

		bool m_VSync = false;

		uint32_t m_MaxFramesInFlight = 2;
		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        std::vector<VkSemaphore> m_ComputeFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;
		std::vector<VkFence> m_ImagesInFlight;
        std::vector<VkFence> m_ComputeInFlightFences;
        
        VkFence m_RayTraceFence;

		uint32_t m_CurrentFrame = 0;
		uint32_t m_CurrentImageIndex = 0;

        uint32_t m_Frame = 0;
        
        bool m_NeedResize = false;
		bool m_IsRecreating = false;
	};

}
