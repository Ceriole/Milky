-- premake5.lua Milky
workspace "Milky"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Milky/vendor/GLFW/include"
IncludeDir["Glad"] = "Milky/vendor/Glad/include"
IncludeDir["ImGui"] = "Milky/vendor/imgui"

group "Dependencies"
	include "Milky/vendor/GLFW"
	include "Milky/vendor/Glad"
	include "Milky/vendor/imgui"
group ""

project "Milky"
	location "Milky"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "Off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "mlpch.h"
	pchsource "Milky/src/mlpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"ML_PLATFORM_WINDOWS",
			"ML_BUILD_DLL",
			"GLFW_INCLUDE_NONE",
			"IMGUI_IMPL_OPENGL_LOADER_CUSTOM"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		defines "ML_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "ML_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "ML_DIST"
		runtime "Release"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Milky/src",
		"Milky/vendor/spdlog/include"
	}

	links
	{
		"Milky"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "Off"
		systemversion "latest"

		defines
		{
			"ML_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "ML_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "ML_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "ML_DIST"
		runtime "Release"
		optimize "On"