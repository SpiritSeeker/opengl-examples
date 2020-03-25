workspace "OpenGL"
	architecture "x64"
	startproject "OpenGL"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	platforms
	{
		"linux",
		"windows"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "OpenGL/vendor/GLFW/include"
IncludeDir["GLEW"] = "OpenGL/vendor/GLEW/include"

include "OpenGL/vendor/GLFW"

project "OpenGL"
	location "OpenGL"
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
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLEW}"
	}

	libdirs
	{
		"OpenGL/vendor/GLEW/lib"
	}

	links
	{
		"GLFW",
		"GLEW:static"
	}

	defines
	{
		"GLEW_STATIC"
	}

	filter "system:linux"
		cppdialect "C++17"
		staticruntime "On"

		links
		{
			"GL",
			"dl",
			"pthread"
		}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		links
		{
			"libopengl32.lib"
		}

	filter "configurations:Debug"
		symbols "On"

	filter "configurations:Release"
		optimize "On"

	filter "configurations:Dist"
		optimize "On"
