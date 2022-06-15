workspace "Neko"
	architecture "x64"

	configurations{
		"Debug",
		"Release",
		"Distribution"
	}


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include Directories relative to root floder 
IncludeDir = {}
IncludeDir["GLFW"] = "Neko/vendor/GLFW/include"

-- include premake file like c++ include (copy text here)
include "Neko/vendor/GLFW"

project "Neko"
	location "Neko"
	kind "SharedLib"
	language "C++"

	targetdir("bin/" ..outputdir.. "/%{prj.name}")
	objdir("bin-int/" ..outputdir.. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "Neko/src/pch.cpp"

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",

	}

	includedirs{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
	}

	links{
		"GLFW",
		"opengl32.lib",
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"

		defines{
			"NEKO_PLATFORM_WINDOWS",
			"NEKO_BUILD_DLL",
		}

		postbuildcommands{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" ..outputdir.. "/SandBox")
		}

	filter "configurations:Debug"
		defines "NEKO_DEBUG"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "NEKO_RELEASE"
		buildoptions "/MD"
		symbols "On"

	filter "configurations:Distribution"
		defines "NEKO_DIST"
		buildoptions "/MD"
		symbols "On"
		
project "SandBox"
	location "SandBox"
	kind "ConsoleApp"
	language "C++"

	targetdir("bin/" ..outputdir.. "/%{prj.name}")
	objdir("bin-int/" ..outputdir.. "/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",

	}

	includedirs{
		"Neko/vendor/spdlog/include",
		"Neko/src",
	}

	links{
		"Neko"
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"

		defines{
			"NEKO_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "NEKO_DEBUG"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "NEKO_RELEASE"
		buildoptions "/MD"
		symbols "On"

	filter "configurations:Distribution"
		defines "NEKO_DIST"
		buildoptions "/MD"
		symbols "On"

