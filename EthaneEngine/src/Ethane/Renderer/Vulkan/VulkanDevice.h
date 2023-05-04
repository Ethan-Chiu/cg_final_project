#pragma once

#include <optional>
#include "Vulkan.h"

namespace Ethane {

	enum class QueueFamilyTypes {
		Graphics = 0,
		Compute = 1,
		Transfer = 2
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanPhysicalDevice
{
public:
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> Graphics;
        std::optional<uint32_t> Compute;
        std::optional<uint32_t> Transfer;
        std::optional<uint32_t> Present;
        
        bool isComplete()
        {
            return Graphics.has_value();
        }
    };
    
public:
		VulkanPhysicalDevice(const std::vector<uint32_t>& compatibleDeviceIndices, VkSurfaceKHR surface);
		~VulkanPhysicalDevice() {};
        
		void Destroy();

		static Scope<VulkanPhysicalDevice> Init(const std::vector<uint32_t>& compatibleDeviceIndices, VkSurfaceKHR surface);

		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device) const;

		// Getter
		VkPhysicalDevice GetVulkanPhysicalDevice() const { return m_PhysicalDevice; }
		const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }

	private:
		int32_t RateDeviceSuitability(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, uint32_t queueFamilyFlags);
		void PrintSelectedDeviceInfo();

	private:
		const uint32_t m_ConstRequestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
		
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

		QueueFamilyIndices m_QueueFamilyIndices;

		VkSurfaceKHR m_Surface; // For selecting device

	friend class VulkanDevice;
	friend class VulkanContext;
	};

	class VulkanDevice
	{
	public:
		VulkanDevice(const VulkanPhysicalDevice* physicalDevice, std::vector<std::string>& usedExtensions, VkPhysicalDeviceFeatures2 enabledFeatures2);
		~VulkanDevice() {};
		void Destroy();

		static Scope<VulkanDevice> Create(const VulkanPhysicalDevice* physicalDevice, std::vector<std::string>& usedExtensions, VkPhysicalDeviceFeatures2 enabledFeatures2);

		void SubmitCommandBuffer(VkCommandBuffer commandBuffer, QueueFamilyTypes type = QueueFamilyTypes::Graphics);

		// Getter
		VkDevice GetVulkanDevice() const { return m_LogicalDevice; }
		const VulkanPhysicalDevice* GetPhysicalDevice() const { return m_PhysicalDevice; }

		VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
		VkQueue GetComputeQueue() const { return m_ComputeQueue; }

		VkCommandPool GetGraphicsCommandPool() const { return m_GraphicsCommandPool; }
		VkCommandPool GetComputeCommandPool() const { return m_ComputeCommandPool; }
	private:
		void QueueCreateInfo();

	private:
		VkDevice m_LogicalDevice = VK_NULL_HANDLE;
        const VulkanPhysicalDevice* m_PhysicalDevice;
		VkPhysicalDeviceFeatures2 m_EnabledFeatures2;

		std::vector<VkDeviceQueueCreateInfo> m_QueueCreateInfos;

		VkQueue m_GraphicsQueue, m_ComputeQueue, m_TransferQueue;

		VkCommandPool m_GraphicsCommandPool, m_ComputeCommandPool;
	};

	namespace Utils
	{
		static const char* VulkanVendorIDToString(uint32_t vendorID)
		{
			switch (vendorID)
			{
			case 0x10DE: return "NVIDIA";
			case 0x1002: return "AMD";
			case 0x8086: return "INTEL";
			case 0x13B5: return "ARM";
			}
			return "Unknown";
		}
	}
}
