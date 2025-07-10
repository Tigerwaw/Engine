include "../../../Premake/common.lua"

workspace "FRAGILE"
  location "%{dirs.root}"
  architecture "x64"
  configurations { "Debug", "Release", "Retail" }

group "Engine"
project "GraphicsEngineShaders"
  location "%{dirs.graphicsengineshaders}"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++20"

  outputdir = path.getrelative(dirs.graphicsengineshaders, dirs.shaders.absolute)

  targetdir (outputdir)
	objdir ("%{dirs.temp}/%{cfg.buildcfg}/%{prj.name}")

  files {
		"**.hlsl",
		"**.hlsli"
	}

  shadermodel("5.0")

	filter("files:**.hlsl")
		flags("ExcludeFromBuild")
		shaderobjectfileoutput(outputdir .."/%{file.basename}.cso")

	filter("files:**VS.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Vertex")

	filter("files:**GS.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Geometry")

  filter("files:**PS.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Pixel")