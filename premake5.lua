-- premake5.lua Milky
include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "Milky"
	architecture "x86_64"
	startproject "MilkGlass"

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

group "Dependencies"
	include "vendor/premake"
	include "Milky/vendor/GLFW"
	include "Milky/vendor/Glad"
	include "Milky/vendor/imgui"
	include "Milky/vendor/yaml-cpp"
group ""

include "Milky"
include "MilkGlass"
include "Sandbox"
