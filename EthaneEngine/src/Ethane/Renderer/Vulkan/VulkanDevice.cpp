#include "ethpch.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"

namespace Ethane {

	Scope<VulkanPhysicalDevice> VulkanPhysicalDevice::Init(const std::vector<uint32_t>& compatibleDeviceIndices, VkSurfaceKHR surface)
	{
		return CreateScope<VulkanPhysicalDevice>(compatibleDeviceIndices, surface);
	}

	VulkanPhysicalDevice::VulkanPhysicalDevice(const std::vector<uint32_t>& compatibleDeviceIndices, VkSurfaceKHR surface)
		:m_Surface(surface)
	{
		auto vkInstance = VulkanContext::GetInstance();

		// Get devices
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());

		// Select device 
		std::multimap<uint32_t, VkPhysicalDevice> candidates;
		for (const auto& deviceIndex : compatibleDeviceIndices)
		{
			int score = RateDeviceSuitability(devices[deviceIndex]);
			candidates.insert(std::make_pair(score, devices[deviceIndex]));
		}
		if (candidates.rbegin()->first >= 0)
		{
			m_PhysicalDevice = candidates.rbegin()->second;
		}
		
        ETH_CORE_ASSERT(m_PhysicalDevice != nullptr, "failed to find a suitable GPU");

		// Queue families
		m_QueueFamilyIndices = FindQueueFamilies(m_PhysicalDevice, m_ConstRequestedQueueTypes);

		PrintSelectedDeviceInfo();
	}

	void VulkanPhysicalDevice::Destroy()
	{
		ETH_CORE_INFO("Destroying physical device...");
		m_QueueFamilyIndices.Graphics.reset();
		m_QueueFamilyIndices.Compute.reset();
		m_QueueFamilyIndices.Transfer.reset();
		m_QueueFamilyIndices.Present.reset();
	}

	int32_t VulkanPhysicalDevice::RateDeviceSuitability(VkPhysicalDevice device)
	{
		int score = 0;
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			score += 1000;
		}

		score += deviceProperties.limits.maxImageDimension2D;

		if (!deviceFeatures.geometryShader)
			return -1;

		QueueFamilyIndices indices = FindQueueFamilies(device, VK_QUEUE_GRAPHICS_BIT);
		ETH_CORE_ASSERT(indices.Present.has_value(), "Present not support");
		// prefer indices.Graphic == indices.Present
		if (indices.Present.value() == indices.Graphics.value()) {
			score += 10;
		}

		// Check swap chain adequate
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
		bool swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

		if (!indices.isComplete() || !deviceFeatures.samplerAnisotropy || !swapChainAdequate)
			return -1;
		
		return score;
	}

	VulkanPhysicalDevice::QueueFamilyIndices VulkanPhysicalDevice::FindQueueFamilies(VkPhysicalDevice device, uint32_t flags)
	{
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		QueueFamilyIndices indices;
		// Dedicated queue for compute
		// Try to find a queue family index that supports compute but not graphics
		if (flags & VK_QUEUE_COMPUTE_BIT)
		{
			for (uint32_t i = 0; i < queueFamilyCount; i++)
			{
				auto& queueFamily = queueFamilies[i];
				if ((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) && ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
				{
					indices.Compute = i;
					break;
				}
			}
		}
		// Dedicated queue for transfer
		// Try to find a queue family index that supports transfer but not graphics and compute
		if (flags & VK_QUEUE_TRANSFER_BIT)
		{
			for (uint32_t i = 0; i < queueFamilyCount; i++)
			{
				auto& queueFamily = queueFamilies[i];
				if ((queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) && ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && ((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
				{
					indices.Transfer = i;
					break;
				}
			}
		}
		// Graphics queue
		// Try to find graphics queue that also support present (indices.Graphic == indices.Present)
		VkBool32 presentSupport = false;
		if (flags & VK_QUEUE_GRAPHICS_BIT)
		{
			for (uint32_t i = 0; i < queueFamilyCount; i++)
			{
				if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					if (!presentSupport)
					{
						VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport));
						if (presentSupport) {
							indices.Graphics = i;
							indices.Present = i;
						}
					}
				}
			}
		}
		// For other queue types or if no separate compute queue is present, return the first one to support the requested flags
		ETH_CORE_ASSERT(m_Surface != nullptr);
		for (uint32_t i = 0; i < queueFamilyCount; i++)
		{
			if ((flags & VK_QUEUE_TRANSFER_BIT) && !indices.Transfer.has_value())
			{
				if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
					indices.Transfer = i;
					ETH_CORE_INFO("fallback transfer queue choice");
				}
			}

			if ((flags & VK_QUEUE_COMPUTE_BIT) && !indices.Compute.has_value())
			{
				if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
					indices.Compute = i;
					ETH_CORE_INFO("fallback compute queue choice");
				}
			}

			if ((flags & VK_QUEUE_GRAPHICS_BIT) && !indices.Graphics.has_value())
			{
				if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
					indices.Graphics = i;
					ETH_CORE_INFO("fallback graphic queue choice");
				}
			}
			if (!indices.Present.has_value())
			{
				VkBool32 fallbackPresentSupport = VK_FALSE;
				VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &fallbackPresentSupport));
				if (fallbackPresentSupport) {
					indices.Present = i;
					ETH_CORE_INFO("fallback present queue choice");
				}
			}
		}

		return indices;
	}

	SwapChainSupportDetails VulkanPhysicalDevice::QuerySwapChainSupport(VkPhysicalDevice device) const
	{
		SwapChainSupportDetails details;

		// query capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);

		// query formats
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);
		ETH_CORE_ASSERT(formatCount > 0, "");
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());

		// query present mode
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, details.presentModes.data());

		return details;
	}

	void VulkanPhysicalDevice::PrintSelectedDeviceInfo() 
	{
		// Get memory properties & properties & feature of the selected device
		VkPhysicalDeviceMemoryProperties memories;
		VkPhysicalDeviceProperties2 properties2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
		vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memories);
		vkGetPhysicalDeviceProperties2(m_PhysicalDevice, &properties2);

		VkPhysicalDeviceProperties properties = properties2.properties;
		ETH_CORE_INFO("____________________");
		ETH_CORE_INFO("SELECTED DEVICE: {0}", properties.deviceName);
		ETH_CORE_INFO("queue family indices: Graphics-{0} | Present-{1} | Compute-{2} | Transfer-{3}", 
			m_QueueFamilyIndices.Graphics.value(), 
			m_QueueFamilyIndices.Present.value(),
			m_QueueFamilyIndices.Compute.value(),
			m_QueueFamilyIndices.Transfer.value());

		switch (properties.deviceType) {
		default:
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
			ETH_CORE_INFO("GPU type is Unknown.");
			break;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			ETH_CORE_INFO("GPU type is Integrated.");
			break;
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			ETH_CORE_INFO("GPU type is Descrete.");
			break;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			ETH_CORE_INFO("GPU type is Virtual.");
			break;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			ETH_CORE_INFO("GPU type is CPU.");
			break;
		}

		ETH_CORE_INFO(
			"GPU Driver version: {0}.{1}.{2}",
			VK_VERSION_MAJOR(properties.driverVersion),
			VK_VERSION_MINOR(properties.driverVersion),
			VK_VERSION_PATCH(properties.driverVersion));

		// Vulkan API version.
		ETH_CORE_INFO(
			"Vulkan API version: {0}.{1}.{2}",
			VK_VERSION_MAJOR(properties.apiVersion),
			VK_VERSION_MINOR(properties.apiVersion),
			VK_VERSION_PATCH(properties.apiVersion));

		// Memory information
		for (uint32_t i = 0; i < memories.memoryHeapCount; ++i) {
			double memory_size_gib = (((double)memories.memoryHeaps[i].size) / 1024.0f / 1024.0f / 1024.0f);
			if (memories.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
				ETH_CORE_INFO("Local GPU memory: {0} GiB", std::round(memory_size_gib * 100.0) / 100.0);
			}
			else {
				ETH_CORE_INFO("Shared System memory: {0} GiB", std::round(memory_size_gib * 100.0) / 100.0);
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Logical device 
	////////////////////////////////////////////////////////////////////////////////////
	Scope<VulkanDevice> VulkanDevice::Create(const VulkanPhysicalDevice* physicalDevice, std::vector<std::string>& usedExtensions, VkPhysicalDeviceFeatures2 enabledFeatures2)
	{
		return CreateScope<VulkanDevice>(physicalDevice, usedExtensions, enabledFeatures2);
	}
	
	VulkanDevice::VulkanDevice(const VulkanPhysicalDevice* physicalDevice, std::vector<std::string>& usedExtensions, VkPhysicalDeviceFeatures2 enabledFeatures2)
		: m_PhysicalDevice(physicalDevice), m_EnabledFeatures2(enabledFeatures2)
	{
		QueueCreateInfo();

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(m_QueueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = m_QueueCreateInfos.data();

		std::vector<const char*> deviceExtensions;
		for (const auto& it : usedExtensions)
		{
			deviceExtensions.push_back(it.c_str());
		}
		if (deviceExtensions.size() > 0)
		{
			deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
			deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
		}

		deviceCreateInfo.pEnabledFeatures = nullptr;
		deviceCreateInfo.pNext = &m_EnabledFeatures2;

		VK_CHECK_RESULT(vkCreateDevice(m_PhysicalDevice->GetVulkanPhysicalDevice(), &deviceCreateInfo, nullptr, &m_LogicalDevice));

		// retrieving queue handles
		vkGetDeviceQueue(m_LogicalDevice, m_PhysicalDevice->m_QueueFamilyIndices.Graphics.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_LogicalDevice, m_PhysicalDevice->m_QueueFamilyIndices.Compute.value(), 0, &m_ComputeQueue);
		vkGetDeviceQueue(m_LogicalDevice, m_PhysicalDevice->m_QueueFamilyIndices.Transfer.value(), 0, &m_TransferQueue);

		// create command pool
		VkCommandPoolCreateInfo cmdPoolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
		cmdPoolInfo.queueFamilyIndex = m_PhysicalDevice->m_QueueFamilyIndices.Graphics.value();
		cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		VK_CHECK_RESULT(vkCreateCommandPool(m_LogicalDevice, &cmdPoolInfo, nullptr, &m_GraphicsCommandPool));

		cmdPoolInfo.queueFamilyIndex = m_PhysicalDevice->m_QueueFamilyIndices.Compute.value();
		VK_CHECK_RESULT(vkCreateCommandPool(m_LogicalDevice, &cmdPoolInfo, nullptr, &m_ComputeCommandPool));
	}

	void VulkanDevice::Destroy()
	{
		// destroy command pools
		vkDestroyCommandPool(m_LogicalDevice, m_GraphicsCommandPool, nullptr);
		vkDestroyCommandPool(m_LogicalDevice, m_ComputeCommandPool, nullptr);

		// reset queue
		m_GraphicsQueue = nullptr;
		m_ComputeQueue = nullptr;
		m_TransferQueue = nullptr;

		ETH_CORE_INFO("Destroying logical device...");
		vkDeviceWaitIdle(m_LogicalDevice);
		vkDestroyDevice(m_LogicalDevice, nullptr);
	}

	void VulkanDevice::QueueCreateInfo()
	{
		VulkanPhysicalDevice::QueueFamilyIndices queueFamilyIndices = m_PhysicalDevice->GetQueueFamilyIndices();
		std::set<uint32_t> uniqueQueueFamilies = {
			queueFamilyIndices.Present.value(),
			queueFamilyIndices.Graphics.value(),
			queueFamilyIndices.Compute.value(),
			queueFamilyIndices.Transfer.value()
		};

		float queuePriority(1.0f);
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			m_QueueCreateInfos.push_back(queueCreateInfo);
		}

	}

	void VulkanDevice::SubmitCommandBuffer(VkCommandBuffer commandBuffer, QueueFamilyTypes type)
	{
		ETH_CORE_ASSERT(commandBuffer != VK_NULL_HANDLE);

		VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		// Create fence to ensure that the command buffer has finished executing
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = 0;
		VkFence fence;
		VK_CHECK_RESULT(vkCreateFence(m_LogicalDevice, &fenceCreateInfo, nullptr, &fence));

		// Submit to the queue
		switch (type)
		{
		case QueueFamilyTypes::Graphics: {VK_CHECK_RESULT(vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, fence)); break; }
		case QueueFamilyTypes::Compute: {VK_CHECK_RESULT(vkQueueSubmit(m_ComputeQueue, 1, &submitInfo, fence)); break; }
		default:
			ETH_CORE_ASSERT(false, "No command pool");
		}

		// Wait for the fence to signal that command buffer has finished executing
		VK_CHECK_RESULT(vkWaitForFences(m_LogicalDevice, 1, &fence, VK_TRUE, UINT64_MAX));
		vkDestroyFence(m_LogicalDevice, fence, nullptr);

		switch (type)
		{
		case QueueFamilyTypes::Graphics: {vkFreeCommandBuffers(m_LogicalDevice, m_GraphicsCommandPool, 1, &commandBuffer); break; }
		case QueueFamilyTypes::Compute: {vkFreeCommandBuffers(m_LogicalDevice, m_ComputeCommandPool, 1, &commandBuffer); break; }
		default:
			ETH_CORE_ASSERT(false, "No command pool");
		}
	}
}
