#include "ethpch.h"
#include "VulkanContext.h"

#include "Vulkan.h"
#include <GLFW/glfw3.h>

namespace Ethane {

	//--------------------------------------------------------------------------------------------------
	//
	void ContextCreateInfo::AddInstanceLayer(const char* name, bool optional)
	{
		InstanceLayers.emplace_back(name, optional);
	}

	void ContextCreateInfo::AddInstanceExtension(const char* name, bool optional)
	{
		InstanceExtensions.emplace_back(name, optional);
	}

	void ContextCreateInfo::AddDeviceExtension(const char* name, bool optional, void* pFeatureStruct, uint32_t version)
	{
		DeviceExtensions.emplace_back(name, optional, pFeatureStruct, version);
	}

	void ContextCreateInfo::RemoveInstanceLayer(const char* name)
	{
		for (size_t i = 0; i < InstanceLayers.size(); i++)
		{
			if (strcmp(InstanceLayers[i].name.c_str(), name) == 0)
			{
				InstanceLayers.erase(InstanceLayers.begin() + i);
			}
		}
	}
	void ContextCreateInfo::RemoveInstanceExtension(const char* name)
	{
		for (size_t i = 0; i < InstanceExtensions.size(); i++)
		{
			if (strcmp(InstanceExtensions[i].name.c_str(), name) == 0)
			{
				InstanceExtensions.erase(InstanceExtensions.begin() + i);
			}
		}
	}

	void ContextCreateInfo::RemoveDeviceExtension(const char* name)
	{
		for (size_t i = 0; i < DeviceExtensions.size(); i++)
		{
			if (strcmp(DeviceExtensions[i].name.c_str(), name) == 0)
			{
				DeviceExtensions.erase(DeviceExtensions.begin() + i);
			}
		}
	}

	//--------------------------------------------------------------------------------------------------
	
#ifdef ETH_DEBUG
	static bool s_Validation = true;
#else
	static bool s_Validation = false;
#endif

	static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
		VkDebugUtilsMessageTypeFlagsEXT message_types,
		const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
		void* user_data) {
		switch (message_severity) {
		default:
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			ETH_CORE_ERROR(callback_data->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			ETH_CORE_WARN(callback_data->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			ETH_CORE_INFO(callback_data->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			ETH_CORE_TRACE(callback_data->pMessage);
			break;
		}
		return VK_FALSE;
	}

	//--------------------------------------------------------------------------------------------------
	// 
	VulkanContext::VulkanContext(GLFWwindow* windowHandle)
		:m_WindowHandle(windowHandle)
	{
		ETH_CORE_ASSERT(windowHandle, "Window handle is null");
	}

	void VulkanContext::Init()
	{
		ETH_CORE_ASSERT(glfwVulkanSupported(), "GLFW vulkan support error");
	}


	void VulkanContext::Shutdown()
	{
	}
}
