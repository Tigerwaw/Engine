include "../../Premake/common.lua"

workspace "TGPGameEngine"
  location "%{dirs.root}"
  architecture "x64"
  configurations { "Debug", "Internal", "Release" }

group "Engine"
project "GameEngine"
  location "%{dirs.engine}"
  kind "StaticLib"
  language "C++"
  cppdialect "C++20"

  targetdir ("%{dirs.lib}" .. outputdir .. "/%{prj.name}")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/" .. outputdir .. "/%{prj.name}")

  pchheader "Enginepch.h"
  pchsource "Enginepch.cpp"

  files {
		"**.h",
		"**.hpp",
		"**.cpp"
	}

  includedirs { 
    dirs.source,
    dirs.engine,
    dirs.graphics,
    dirs.graphicsengine,
    dirs.assetmanager,
    dirs.utilities,
    dirs.imgui
  }

  filter "system:windows"
    cppdialect "C++20"
    systemversion "latest"
    warnings "Extra"
    flags { 
		"FatalCompileWarnings",
		"MultiProcessorCompile"
    }
  filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"
  filter "configurations:Internal"
		defines "_INTERNAL"
		runtime "Release"
		optimize "on"
  filter "configurations:Release"
	  defines "_RELEASE"
	  runtime "Release"
	  optimize "on"