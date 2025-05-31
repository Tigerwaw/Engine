include "../../Premake/common.lua"

workspace "FRAGILE"
  location "%{dirs.root}"
  architecture "x64"
  configurations { "Debug", "Release", "Retail" }

group "Engine"
project "AssetManager"
  location "%{dirs.assetmanager}"
  kind "StaticLib"
  language "C++"
  cppdialect "C++20"
  enableunitybuild "On"
  conformancemode "On"

	dependson {
    "Math",
    "CommonUtilities",
    "Logger",
    "GameEngine", -- Remove
    "GraphicsEngine" -- Remove
  }

  targetdir ("%{dirs.lib}/%{cfg.buildcfg}/%{prj.name}")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{cfg.buildcfg}/%{prj.name}")

  pchheader "Assetpch.h"
  pchsource "Assetpch.cpp"

  files {
		"**.h",
		"**.hpp",
		"**.cpp"
	}

  includedirs { 
    dirs.assetmanager,
    dirs.utilities,
    dirs.graphicsengine, -- Remove
    dirs.engine -- Remove
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
  filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "on"
  filter "configurations:Retail"
	  defines "_RETAIL"
	  runtime "Release"
	  optimize "on"