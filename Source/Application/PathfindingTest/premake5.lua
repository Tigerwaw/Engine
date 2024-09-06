include "../../../Premake/common.lua"

workspace "TGPGameEngine"
  location "%{dirs.root}"
  architecture "x64"
  configurations { "Debug", "Release", "Retail" }

group "Applications"
project "PathfindingTest"
  location "%{dirs.application}/%{prj.name}/"
  kind "WindowedApp"
  language "C++"
  cppdialect "C++20"

  dependson { 
    "GameEngine", 
    "GraphicsEngine", 
    "GraphicsEngineShaders", 
    "AssetManager",
    "Imgui",
    "Logger"
  }

  debugdir "%{dirs.bin}/%{prj.name}"
  targetdir ("%{dirs.bin}/%{prj.name}")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{cfg.buildcfg}/%{prj.name}")

  files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"**.hlsl",
		"**.hlsli",
    "**.rc"
	}

  includedirs { 
    dirs.source,
    dirs.application,
    dirs.engine,
    dirs.graphics,
    dirs.graphicsengine,
    dirs.assetmanager,
    dirs.utilities,
    dirs.imgui
  }

  libdirs { dirs.lib .. "%{cfg.buildcfg}/**" }
  links { 
    "AssetManager_%{cfg.buildcfg}", 
    "GameEngine_%{cfg.buildcfg}", 
    "GraphicsEngine_%{cfg.buildcfg}", 
    "Imgui_%{cfg.buildcfg}", 
    "Logger_%{cfg.buildcfg}",
    "TGAFbx",
    "d3d11.lib",
    "dxguid.lib",
    "dxgi.lib",
    "d3dcompiler.lib"
  }

  verify_or_create_settings("PathfindingTest")

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
    links { "fmodL_vc", "fmodstudioL_vc" }
  filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "on"
    links { "fmod_vc", "fmodstudio_vc" }
  filter "configurations:Retail"
	  defines "_RETAIL"
	  runtime "Release"
	  optimize "on"
    links { "fmod_vc", "fmodstudio_vc" }

  shadermodel("5.0")

	filter("files:**.hlsl")
		flags("ExcludeFromBuild")
		shaderobjectfileoutput(dirs.shaders.absolute .."/SH_%{file.basename}.cso")

	filter("files:**VS.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Vertex")

	filter("files:**GS.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Geometry")

  filter("files:**PS.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Pixel")