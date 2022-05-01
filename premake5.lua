-- premake5.lua Milky
include "./vendor/premake/premake_customization/solution_items.lua"

workspace "Milky"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Milky/vendor/GLFW/include"
IncludeDir["Glad"] = "Milky/vendor/Glad/include"
IncludeDir["ImGui"] = "Milky/vendor/imgui"
IncludeDir["glm"] = "Milky/vendor/glm"
IncludeDir["stb_image"] = "Milky/vendor/stb_image"

group "Dependencies"
	include "Milky/vendor/GLFW"
	include "Milky/vendor/Glad"
	include "Milky/vendor/imgui"
group ""

project "Milky"
	location "Milky"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "mlpch.h"
	pchsource "%{prj.name}/src/mlpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}"
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
			"ML_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "ML_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ML_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ML_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	includedirs
	{
		"Milky/vendor/spdlog/include",
		"Milky/src",
		"Milky/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Milky"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"ML_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "ML_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ML_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ML_DIST"
		runtime "Release"
		optimize "on"
