include "../../../Premake/common.lua"

workspace "TGPGameEngine"
  location "%{dirs.root}"
  architecture "x64"
  configurations { "Debug", "Release", "Retail" }

group "Engine"
project "GraphicsEngine"
  location "%{dirs.graphicsengine}"
  kind "StaticLib"
  language "C++"
  cppdialect "C++20"
  enableunitybuild "On"
  conformancemode "On"

  targetdir ("%{dirs.lib}%{cfg.buildcfg}/%{prj.name}")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{cfg.buildcfg}/%{prj.name}")

  pchheader "GraphicsEngine.pch.h"
  pchsource "GraphicsEngine.pch.cpp"

  files {
		"**.h",
		"**.hpp",
		"**.cpp"
	}

    dependson {
    "GraphicsEngineShaders",
    "Math",
    "CommonUtilities",
    "Imgui",
    "Logger"
  }

  includedirs {
    dirs.graphicsengine,
    dirs.assetmanager,
    dirs.utilities,
    dirs.imgui,
    dirs.commonutilities,
    dirs.math,
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