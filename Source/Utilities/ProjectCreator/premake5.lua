include "../../../Premake/common.lua"

workspace "EngineUtilities"
  location "%{dirs.projectcreator}"
  architecture "x64"
  configurations { "Debug", "Release" }

project "ProjectCreator"
  location "%{dirs.utilities}/%{prj.name}/"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++20"

  debugdir "%{dirs.root}"
  targetdir ("%{dirs.root}")
	targetname("%{prj.name}")
	objdir ("%{dirs.temp}/%{cfg.buildcfg}/%{prj.name}")

  files {
		"%{dirs.utilities}/%{prj.name}/*.h",
		"%{dirs.utilities}/%{prj.name}/*.cpp",
		"%{dirs.utilities}/%{prj.name}/*.rc"
    
	}

  includedirs { 
    dirs.utilities .. "/ProjectCreator/"
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