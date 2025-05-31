include "../../../Premake/common.lua"

workspace "FRAGILE"
  location "%{dirs.root}"
  architecture "x64"
  configurations { "Debug", "Release", "Retail" }

group "Utilities"
project "Logger"
  location "%{dirs.logger}"
  kind "StaticLib"
  language "C++"
  cppdialect "C++20"

  debugdir "%{dirs.bin}"
  targetdir ("%{dirs.lib}%{cfg.buildcfg}/%{prj.name}")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{cfg.buildcfg}/%{prj.name}")

  files {
		"**.h",
		"**.cpp"
	}

  includedirs { 
    dirs.utilities .. "/Logger/"
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