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
        
        //--------------------------------------------------------------------------------------------------
        // Setup instance extensions/layers & device extension
        uint32_t required_extension_count = 0;
        const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&required_extension_count);
        
        ContextCreateInfo contextCreateInfo;
        for (uint8_t i = 0; i < required_extension_count; i++)
        {
            ETH_CORE_TRACE("{0} is needed by glfw", glfw_extensions[i]);
            contextCreateInfo.AddInstanceExtension(glfw_extensions[i]);
        }
        contextCreateInfo.AddDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        contextCreateInfo.AddDeviceExtension(VK_EXT_DEBUG_MARKER_EXTENSION_NAME, true);
        if (s_Validation)
        {
            contextCreateInfo.AddInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            contextCreateInfo.AddInstanceExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
            const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
            contextCreateInfo.AddInstanceLayer(validationLayerName);
            const char* fpsLayerName = "VK_LAYER_LUNARG_monitor";
            contextCreateInfo.AddInstanceLayer(fpsLayerName, true);
        }
        // RayTracing: Activate the ray tracing extension
        VkPhysicalDeviceAccelerationStructureFeaturesKHR accelFeature{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR };
        contextCreateInfo.AddDeviceExtension(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, false, &accelFeature);  // To build acceleration structures
        VkPhysicalDeviceRayTracingPipelineFeaturesKHR rtPipelineFeature{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR };
        contextCreateInfo.AddDeviceExtension(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME, false, &rtPipelineFeature);  // To use vkCmdTraceRaysKHR
        contextCreateInfo.AddDeviceExtension(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);  // Required by ray tracing pipeline

        // NOTE: use this to enable validation features
        VkValidationFeatureEnableEXT enables[] = { VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT };
        VkValidationFeaturesEXT features = { VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT };
        features.enabledValidationFeatureCount = 1;
        features.pEnabledValidationFeatures = enables;
        // contextCreateInfo.InstanceCreateInfoExt = &features;

        //--------------------------------------------------------------------------------------------------
        // Create Instance
        if (!InitInstance(contextCreateInfo)) {
            ETH_CORE_ERROR("Instance creation failed!");
        }
        
        //--------------------------------------------------------------------------------------------------
        // Surface create
        if (CreateSurface() != VK_SUCCESS) {
            ETH_CORE_ERROR("Surface creation failed");
        }

        //--------------------------------------------------------------------------------------------------
        // Get compatible devices
        auto compatibleDevices = GetCompatibleDevices(contextCreateInfo);
        if (compatibleDevices.empty()) {
            ETH_CORE_ERROR("No compatible device found");
        }

        //--------------------------------------------------------------------------------------------------
        // Physical device & logical device
//        if (!InitDevice(contextCreateInfo, compatibleDevices)) {
//            ETH_CORE_ERROR("Device initialization failed");
//        }

        //--------------------------------------------------------------------------------------------------
        // Swapchain cerate
//        uint32_t width = 1280, height = 720;
//        m_SwapChain = VulkanSwapChain::Create(m_Surface, m_Device, width, height, false);
//        m_SwapChain->Init();
	}


	void VulkanContext::Shutdown()
	{
	}

    //--------------------------------------------------------------------------------------------------
    // Create the Vulkan instance
    //
    bool VulkanContext::InitInstance(const ContextCreateInfo& info)
    {
        // Application Info
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = info.AppTitle.c_str();
        appInfo.pEngineName = info.AppEngine.c_str();
        appInfo.apiVersion = VK_MAKE_VERSION(info.ApiMajor, info.ApiMinor, 0);

        if (info.VerboseUsed)
        {
            uint32_t version;
            VkResult result = vkEnumerateInstanceVersion(&version);
            VK_CHECK_RESULT(result);
            ETH_CORE_INFO("_______________");
            ETH_CORE_INFO("Vulkan Version:");
            ETH_CORE_INFO(" - available:  {0}.{1}.{2}", VK_VERSION_MAJOR(version), VK_VERSION_MINOR(version), VK_VERSION_PATCH(version));
            ETH_CORE_INFO(" - requesting: {0}.{1}.{2}", info.ApiMajor, info.ApiMinor, 0);
        }

        {
            // Get all layers
            // Check if this layer is available at instance level
            uint32_t instanceLayerCount;
            VK_CHECK_RESULT(vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr));
            std::vector<VkLayerProperties> instanceLayerProperties(instanceLayerCount);
            VK_CHECK_RESULT(vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayerProperties.data()));

            if (FillFilteredNameArray(m_UsedInstanceLayers, instanceLayerProperties, info.InstanceLayers) != VK_SUCCESS)
            {
                return false;
            }
                
            // Log available properties
            if (info.VerboseAvailable)
            {
                ETH_CORE_TRACE("Vulkan Instance Layers:");
                for (const VkLayerProperties& layer : instanceLayerProperties)
                {
                    ETH_CORE_TRACE("  {0}", layer.layerName, VK_VERSION_MAJOR(layer.specVersion),
                        VK_VERSION_MINOR(layer.specVersion), VK_VERSION_PATCH(layer.specVersion), layer.implementationVersion, layer.description);
                }
            }
        }

        {
            // Get all extensions
            uint32_t extensionCount;
            VK_CHECK_RESULT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr));
            std::vector<VkExtensionProperties> extensionProperties(extensionCount);
            VK_CHECK_RESULT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProperties.data()));

            std::vector<void*> featureStructs;
            if (FillFilteredNameArray(m_UsedInstanceExtensions, extensionProperties, info.InstanceExtensions, featureStructs) != VK_SUCCESS)
            {
                return false;
            }

            if (info.VerboseAvailable)
            {
                ETH_CORE_INFO("Available Instance Extensions :");
                for (auto extension : extensionProperties)
                {
                    ETH_CORE_INFO("  {0} (v. {1})", std::string(extension.extensionName), extension.specVersion);
                }
            }
        }


        if (info.VerboseUsed)
        {
            ETH_CORE_INFO("______________________");
            ETH_CORE_INFO("Used Instance Layers :");
            for (const auto& it : m_UsedInstanceLayers)
            {
                ETH_CORE_INFO("  {0}", it.c_str());
            }
            ETH_CORE_INFO("Used Instance Extensions :");
            for (const auto& it : m_UsedInstanceExtensions)
            {
                ETH_CORE_INFO("  {0}", it.c_str());
            }
            ETH_CORE_INFO("______________________");
        }


        std::vector<const char*> usedInstanceExtensions;
        std::vector<const char*> usedInstanceLayers;
        for (const auto& it : m_UsedInstanceExtensions)
        {
            usedInstanceExtensions.push_back(it.c_str());
        }
        for (const auto& it : m_UsedInstanceLayers)
        {
            usedInstanceLayers.push_back(it.c_str());
        }

        VkInstanceCreateInfo instanceCreateInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        instanceCreateInfo.pApplicationInfo = &appInfo;
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(usedInstanceExtensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = usedInstanceExtensions.data();
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(usedInstanceLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = usedInstanceLayers.data();
        instanceCreateInfo.pNext = info.InstanceCreateInfoExt;

        VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &s_VulkanInstance));

        // Debug messenger
        for (const auto& it : usedInstanceExtensions)
        {
            if (strcmp(it, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
            {
                InitDebugUtils();
                break;
            }
        }

        return true;
    }

    //--------------------------------------------------------------------------------------------------
    // Create surface
    //
    VkResult VulkanContext::CreateSurface()
    {
        return glfwCreateWindowSurface(s_VulkanInstance, m_WindowHandle, nullptr, &m_Surface);
    }



    //--------------------------------------------------------------------------------------------------
    // Utility function
    //
    //--------------------------------------------------------------------------------------------------
    // Debug messenger
    void VulkanContext::InitDebugUtils() {
        uint32_t log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT; // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        debugCreateInfo.messageSeverity = log_severity;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        debugCreateInfo.pfnUserCallback = VulkanDebugCallback;

        auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(s_VulkanInstance, "vkCreateDebugUtilsMessengerEXT");
        ETH_CORE_ASSERT(vkCreateDebugUtilsMessengerEXT != NULL, "Failed to create debug messenger!");
        VK_CHECK_RESULT(vkCreateDebugUtilsMessengerEXT(s_VulkanInstance, &debugCreateInfo, nullptr, &m_DebugMessenger));
        ETH_CORE_INFO("Vulkan debugger created.");
    }

    VkResult VulkanContext::FillFilteredNameArray(std::vector<std::string>& used,
        const std::vector<VkLayerProperties>& properties,
        const ContextCreateInfo::EntryArray& requested)
    {
        for (const auto& itr : requested)
        {
            bool found = false;
            for (const auto& property : properties)
            {
                if (strcmp(itr.name.c_str(), property.layerName) == 0)
                {
                    found = true;
                    break;
                }
            }

            if (found)
            {
                used.push_back(itr.name);
            }
            else if (itr.optional == false)
            {
                ETH_CORE_ERROR("Requiered layer not found: {0}", itr.name);
                return VK_ERROR_LAYER_NOT_PRESENT;
            }
        }

        return VK_SUCCESS;
    }

    VkResult VulkanContext::FillFilteredNameArray(std::vector<std::string>& used,
        const std::vector<VkExtensionProperties>& properties,
        const ContextCreateInfo::EntryArray& requested,
        std::vector<void*>& featureStructs)
    {
        for (const auto& itr : requested)
        {
            bool found = false;
            for (const auto& property : properties)
            {
                if (strcmp(itr.name.c_str(), property.extensionName) == 0 && (itr.version == 0 || itr.version == property.specVersion))
                {
                    found = true;
                    break;
                }
            }

            if (found)
            {
                used.push_back(itr.name);
                if (itr.pFeatureStruct)
                {
                    featureStructs.push_back(itr.pFeatureStruct);
                }
            }
            else if (!itr.optional)
            {
                ETH_CORE_ERROR("VK_ERROR_EXTENSION_NOT_PRESENT: {0} - {1}", itr.name, itr.version);
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }

        return VK_SUCCESS;
    }


    //--------------------------------------------------------------------------------------------------
    // Returns the list of devices or groups compatible with the mandatory extensions
    //
    std::vector<uint32_t> VulkanContext::GetCompatibleDevices(const ContextCreateInfo& info)
    {
        ETH_CORE_ASSERT(s_VulkanInstance != nullptr);

        std::vector<uint32_t> compatibleDevices;
        
        // std::vector<VkPhysicalDeviceGroupProperties> groups;

        // TODO: investigate device group
        // if (info.useDeviceGroups)
        // {
        //     groups = getPhysicalDeviceGroups();
        //     nbElems = static_cast<uint32_t>(groups.size());
        // }
        // else
        
        // List devices
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(s_VulkanInstance, &deviceCount, nullptr);
        ETH_CORE_ASSERT(deviceCount > 0, "failed to find GPUs with Vulkan support!");
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(s_VulkanInstance, &deviceCount, devices.data());

        if (info.VerboseCompatibleDevices)
        {
            ETH_CORE_INFO("____________________");
            ETH_CORE_INFO("Compatible Devices :");
        }

        uint32_t compatible = 0;
        for (uint32_t elemId = 0; elemId < deviceCount; elemId++)
        {
            VkPhysicalDevice physicalDevice = devices[elemId]; // info.useDeviceGroups ? groups[elemId].devices[0] :

            // Note: all physical devices in a group are identical
            if (HasMandatoryExtensions(physicalDevice, info, info.VerboseCompatibleDevices))
            {
                compatibleDevices.push_back(elemId);
                if (info.VerboseCompatibleDevices)
                {
                    VkPhysicalDeviceProperties props;
                    vkGetPhysicalDeviceProperties(physicalDevice, &props);
                    ETH_CORE_INFO("  {0}: {1}", compatible, props.deviceName);
                    compatible++;
                }
            }
            else if (info.VerboseCompatibleDevices)
            {
                VkPhysicalDeviceProperties props;
                vkGetPhysicalDeviceProperties(physicalDevice, &props);
                ETH_CORE_INFO("Skipping physical device {0}", props.deviceName);
            }
        }
        if (info.VerboseCompatibleDevices)
        {
            ETH_CORE_INFO("Physical devices found: {0}", compatible);
            if (compatible > 0)
            {
                ETH_CORE_INFO("{0}", compatible);
            }
            else
            {
                ETH_CORE_ERROR("No compatible device");
            }
        }

        return compatibleDevices;
    }


    //--------------------------------------------------------------------------------------------------
    // Return true if all extensions in info, marked as required are available on the physicalDevice
    //
    bool VulkanContext::HasMandatoryExtensions(VkPhysicalDevice physicalDevice, const ContextCreateInfo& info, bool bVerbose)
    {
        std::vector<VkExtensionProperties> extensionProperties;

        uint32_t count;
        VK_CHECK_RESULT(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, nullptr));
        extensionProperties.resize(count);
        VK_CHECK_RESULT(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, extensionProperties.data()));
        extensionProperties.resize(std::min(extensionProperties.size(), size_t(count)));

        return CheckEntryArray(extensionProperties, info.DeviceExtensions, bVerbose);
    }


    //--------------------------------------------------------------------------------------------------
    //
    bool VulkanContext::CheckEntryArray(const std::vector<VkExtensionProperties>& properties, const ContextCreateInfo::EntryArray& requested, bool bVerbose)
    {
        for (const auto& itr : requested)
        {
            bool found = false;
            for (const auto& property : properties)
            {
                if (strcmp(itr.name.c_str(), property.extensionName) == 0)
                {
                    found = true;
                    break;
                }
            }

            if (!found && !itr.optional)
            {
                if (bVerbose)
                {
                    ETH_CORE_INFO("Could NOT locate mandatory extension '{0}'", itr.name.c_str());
                }
                return false;
            }
        }

        return true;
    }
}
