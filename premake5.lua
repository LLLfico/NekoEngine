workspace "Neko"
	architecture "x64"

	configurations{
		"Debug",
		"Release",
		"Distribution"
	}

	startproject "SandBox"


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include Directories relative to root floder 
IncludeDir = {}
IncludeDir["GLFW"] = "Neko/vendor/GLFW/include"
IncludeDir["Glad"] = "Neko/vendor/Glad/include"
IncludeDir["ImGui"] = "Neko/vendor/imgui"
IncludeDir["glm"] = "Neko/vendor/glm"
IncludeDir["stb_image"] = "Neko/vendor/stb_image"
IncludeDir["entt"] = "Neko/vendor/entt/include"

-- include premake file like c++ include (copy text here)
include "Neko/vendor/GLFW"
include "Neko/vendor/Glad"
include "Neko/vendor/imgui"


project "Neko"
	location "Neko"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir("bin/" ..outputdir.. "/%{prj.name}")
	objdir("bin-int/" ..outputdir.. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "Neko/src/pch.cpp"

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/glm/glm/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
	}

	links{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib",
	}

	filter "system:windows"
		systemversion "latest"

		defines{
			"NEKO_PLATFORM_WINDOWS",
			"NEKO_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "NEKO_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "NEKO_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Distribution"
		defines "NEKO_DIST"
		runtime "Release"
		optimize "on"
		
project "SandBox"
	location "SandBox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir("bin/" ..outputdir.. "/%{prj.name}")
	objdir("bin-int/" ..outputdir.. "/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs{
		"Neko/vendor/spdlog/include",
		"Neko/src",
		"Neko/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
	}

	links{
		"Neko"
	}

	filter "system:windows"
		systemversion "latest"
		runtime "Debug"
		defines{
			"NEKO_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "NEKO_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "NEKO_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Distribution"
		defines "NEKO_DIST"
		runtime "Release"
		optimize "on"

