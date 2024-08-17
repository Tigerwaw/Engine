include "../../Premake/common.lua"

workspace "TGPGameEngine"
  location "%{dirs.root}"
  architecture "x64"
  configurations { "Debug", "Internal", "Release" }

group "Engine"
project "AssetManager"
  location "%{dirs.assetmanager}"
  kind "StaticLib"
  language "C++"
  cppdialect "C++20"

  targetdir ("%{dirs.lib}/" .. outputdir .. "/%{prj.name}")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/" .. outputdir .. "/%{prj.name}")

  pchheader "Assetpch.h"
  pchsource "Assetpch.cpp"

  files {
		"**.h",
		"**.hpp",
		"**.cpp"
	}

  includedirs { 
    dirs.source,
    dirs.assetmanager,
    dirs.graphicsengine,
    dirs.utilities,
    dirs.engine
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