-- EthaneEngine Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/EthaneEngine/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/EthaneEngine/vendor/Glad/include"
IncludeDir["spdlog"] = "%{wks.location}/EthaneEngine/vendor/spdlog/include"
IncludeDir["glm"] = "%{wks.location}/EthaneEngine/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/EthaneEngine/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/EthaneEngine/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/EthaneEngine/vendor/yaml-cpp/include"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
-- IncludeDir["shaderc"] = "%{wks.location}/EthaneEngine/vendor/shaderc/include"
-- IncludeDir["SPIRV_Cross"] = "%{wks.location}/EthaneEngine/vendor/SPIRV-Cross"
IncludeDir["Assimp"] = "%{wks.location}/EthaneEngine/vendor/assimp/include"
IncludeDir["ImGui"] = "%{wks.location}/EthaneEngine/vendor/imgui"
IncludeDir["ImGuizmo"] = "%{wks.location}/EthaneEngine/vendor/ImGuizmo"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Debug"] = "%{wks.location}/EthaneEngine/vendor/VulkanSDK/Lib"
LibraryDir["VulkanSDK_DebugDLL"] = "%{wks.location}/EthaneEngine/vendor/VulkanSDK/Bin"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

-- Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
-- Library["ShaderC_Utils_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_utild.lib"
-- Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
-- Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
-- Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

-- Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
-- Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
-- Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

-- Library["SPIRV_Cross_Debug"] = "%{wks.location}/EthaneEngine/vendor/SPIRV-Cross/lib/spirv-cross-cored.lib"
-- Library["SPIRV_Cross_Release"] = "%{wks.location}/EthaneEngine/vendor/SPIRV-Cross/lib/spirv-cross-core.lib"
-- Library["SPIRV_Cross_GLSL_Debug"] = "%{wks.location}/EthaneEngine/vendor/SPIRV-Cross/lib/spirv-cross-glsld.lib"
-- Library["SPIRV_Cross_GLSL_Release"] = "%{wks.location}/EthaneEngine/vendor/SPIRV-Cross/lib/spirv-cross-glsl.lib"
