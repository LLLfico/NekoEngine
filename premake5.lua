workspace "Neko"
	architecture "x86_64"

	configurations{
		"Debug",
		"Release",
		"Distribution"
	}
	flags
	{
		"MultiProcessorCompile"
	}

	startproject "Editor"


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include Directories relative to root floder 
IncludeDir = {}
IncludeDir["GLFW"] = "Neko/vendor/GLFW/include"
IncludeDir["Glad"] = "Neko/vendor/Glad/include"
IncludeDir["ImGui"] = "Neko/vendor/imgui"
IncludeDir["glm"] = "Neko/vendor/glm"
IncludeDir["stb_image"] = "Neko/vendor/stb_image"
IncludeDir["entt"] = "Neko/vendor/entt/include"
IncludeDir["yaml_cpp"] = "Neko/vendor/yaml-cpp/include"
IncludeDir["ImGuizmo"] = "Neko/vendor/ImGuizmo"

-- include premake file like c++ include (copy text here)
include "Neko/vendor/GLFW"
include "Neko/vendor/Glad"
include "Neko/vendor/imgui"
include "Neko/vendor/yaml-cpp"


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
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.h",
		"%{prj.name}/vendor/ImGuizmo/ImGuizmo.cpp",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
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
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGuizmo}",
	}

	links{
		"GLFW",
		"Glad",
		"ImGui",
		"yaml-cpp",
		"opengl32.lib",
	}
	filter "files:Neko/vendor/ImGuizmo/**.cpp"
	flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"

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
		"%{IncludeDir.ImGuizmo}",
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


project "Editor"
	location "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
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