project "MilkGlass"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}
	includedirs
	{
		"%{wks.location}/Milky/vendor/spdlog/include",
		"%{wks.location}/Milky/src",
		"%{wks.location}/Milky/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}"
	}

	links
	{
		"Milky"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
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
