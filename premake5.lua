include "Dependencies.lua"

workspace "EthaneEngine"
	architecture "x86_64"
	startproject "Ethane-Editor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


group "Dependencies"
	include "EthaneEngine/vendor/GLFW"
	include "EthaneEngine/vendor/Glad"

group ""
    project "EthaneEngine"

group ""
    project "Ethane-Editor"


-------------------------------------------------------------------------------
-- Ethane Engine ( main library )
-------------------------------------------------------------------------------
project "EthaneEngine"

	location "EthaneEngine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchsource "%{prj.name}/src/ethpch.cpp"

	files
	{
		"%{prj.name}/src/**.h", 
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/entt/include/**.hpp"
	}

    externalincludedirs
    {
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.VulkanSDK}",
		"%{IncludeDir.Assimp}",
		"%{IncludeDir.yaml_cpp}",
    }

	includedirs
	{
		"%{wks.location}/EthaneEngine/src",
	}

	libdirs {"%{LibraryDir.VulkanSDK}"}
	
	links
	{
		"GLFW",
		"Glad",
		"yaml-cpp",
	}
 
    filter "action:xcode4"
        pchheader "src/ethpch.h"
    filter "action:not xcode4"
        pchheader "ethpch.h"

	filter "files:EthaneEngine/vendor/ImGuizmo/**.cpp"
	flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"ETH_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "system:macosx"
		defines {
			"CFG_MACOS",
			"__APPLE__"
		}
		kind "SharedLib"

		libdirs { 
			LibraryDir["MacAssimp"],
			LibraryDir["MacYamlCpp"]
		}

		links {
			"OpenGL.framework",
			"Cocoa.framework",
			"IOKit.framework",
            "vulkan",
            "shaderc_shared",
            "shaderc_util",
            "spirv-cross-core",
            "spirv-cross-glsl",
            "SPIRV-Tools",
			"assimp",
			"z"
		}
  
    filter "configurations:Debug"
        defines "ETH_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "ETH_RELEASE"
        runtime "Release"
        optimize "on"
        
    filter "configurations:Dist"
        defines "ETH_DIST"
        runtime "Release"
        symbols "on"
        
        
-------------------------------------------------------------------------------
-- Ethane Editor
-------------------------------------------------------------------------------
project "Ethane-Editor"

	location "Ethane-Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h", 
		"%{prj.name}/src/**.cpp"
	}

	externalincludedirs 
    { 
		"%{wks.location}/EthaneEngine/src",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
    }

	includedirs
	{
	}

	links
	{
		"EthaneEngine"
	}

	filter "system:windows"
		systemversion "latest"
		
    filter "system:macosx"
        defines {
            "ETH_DEBUG"
        }
  
	filter "configurations:Debug"
		defines "ETH_DEBUG"
		runtime "Debug"
		symbols "on"
		-- links
		-- {
		-- 	"EthaneEngine/vendor/assimp/bin/Debug/assimp-vc141-mtd.lib"
		-- }
		-- postbuildcommands 
		-- {
		-- 	'{COPY} "../EthaneEngine/vendor/assimp/bin/Debug/assimp-vc141-mtd.dll" "%{cfg.targetdir}"',
		-- 	-- '{COPY} "../EthaneEngine/vendor/VulkanSDK/Bin/shaderc_sharedd.dll" "%{cfg.targetdir}"'
		-- }

	filter "configurations:Release"
		defines "ETH_RELEASE"
		runtime "Release"
		optimize "on"
		-- links
		-- {
		-- 	"EthaneEngine/vendor/assimp/bin/Release/assimp-vc141-mt.lib"
		-- }
		-- postbuildcommands 
		-- {
		-- 	'{COPY} "../EthaneEngine/vendor/assimp/bin/Release/assimp-vc141-mt.dll" "%{cfg.targetdir}"',
		-- }

	filter "configurations:Dist"
		defines "ETH_DIST"
		runtime "Release"
		symbols "on"
		-- links
		-- {
		-- 	"EthaneEngine/vendor/assimp/bin/Release/assimp-vc141-mt.lib"
		-- }
		-- postbuildcommands 
		-- {
		-- 	'{COPY} "../EthaneEngine/vendor/assimp/bin/Release/assimp-vc141-mtd.dll" "%{cfg.targetdir}"',
		-- }
