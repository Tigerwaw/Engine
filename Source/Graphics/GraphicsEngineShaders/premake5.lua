include "../../../Premake/common.lua"

workspace "TGPGameEngine"
  location "%{dirs.root}"
  architecture "x64"
  configurations { "Debug", "Internal", "Release" }

group "Engine"
project "GraphicsEngineShaders"
  location "%{dirs.graphicsengineshaders}"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++20"

  targetdir ("%{dirs.shaders.absolute}")
	objdir ("%{dirs.temp}/".. outputdir .."/%{prj.name}")

  files {
		"**.hlsl",
		"**.hlsli"
	}

  shadermodel("5.0")

	filter("files:**.hlsl")
		flags("ExcludeFromBuild")
		shaderobjectfileoutput(dirs.shaders.absolute .."/%{file.basename}.cso")

	filter("files:**VS.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Vertex")

	filter("files:**GS.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Geometry")

  filter("files:**PS.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Pixel")