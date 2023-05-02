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

	pchheader "ethpch.h"
	pchsource "%{prj.name}/src/ethpch.cpp"

	files
	{
		"%{prj.name}/src/**.h", 
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",

		"%{prj.name}/vendor/entt/include/**.hpp",
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
		-- "%{IncludeDir.VulkanSDK}",
	}

	links
	{
		"GLFW",
		"Glad",
		-- "%{Library.Vulkan}",
		-- "%{Library.VulkanUtils}",
	}

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
		
		removefiles
		{
			"%{prj.name}/src/Ethane/Platform/Windows/**"
		}

		links {
			"OpenGL.framework",
			"Cocoa.framework",
			"OpenGL.framework",
			"IOKit.framework",
			"CoreFoundation.framework"
		}

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

	includedirs
	{
		"EthaneEngine/vendor/spdlog/include",
		"EthaneEngine/src",
		"EthaneEngine/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
	}

	links
	{
		"EthaneEngine"
	}

	filter "system:windows"
		systemversion "latest"
		
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
