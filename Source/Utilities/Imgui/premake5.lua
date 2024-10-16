include "../../../Premake/common.lua"

workspace "TGPGameEngine"
  location "%{dirs.root}"
  architecture "x64"
  configurations { "Debug", "Release", "Retail" }

group "Utilities"
project "Imgui"
  location "%{dirs.utilities}/%{prj.name}/"
  kind "StaticLib"
  language "C++"
  cppdialect "C++20"

  targetdir ("%{dirs.lib}%{cfg.buildcfg}/%{prj.name}")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{cfg.buildcfg}/%{prj.name}")

  files {
    "%{dirs.imgui}/backends/imgui_impl_win32.h",
    "%{dirs.imgui}/backends/imgui_impl_win32.cpp",
    "%{dirs.imgui}/backends/imgui_impl_dx11.h",
    "%{dirs.imgui}/backends/imgui_impl_dx11.cpp",
    "%{dirs.imgui}/misc/debuggers/imgui.natvis",
    "%{dirs.imgui}/misc/debuggers/imgui.natstepfilter",
    "%{dirs.imgui}/misc/cpp/imgui_stdlib.h",
    "%{dirs.imgui}/misc/cpp/imgui_stdlib.cpp",
	"%{dirs.imgui}/*.h",
	"%{dirs.imgui}/*.cpp"
	}

  includedirs { 
    dirs.utilities,
    dirs.imgui
  }

  filter "system:windows"
    cppdialect "C++20"
    systemversion "latest"
  filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"
  filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "on"
  filter "configurations:Retail"
	  defines "_RETAIL"
	  runtime "Release"
	  optimize "on"
