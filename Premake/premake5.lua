include "common.lua"

workspace "TGPGameEngine"
  location "%{dirs.root}"
  architecture "x64"
  configurations { "Debug", "Release", "Retail" }
  startproject "FeatureShowcase"

include (dirs.engine)
include (dirs.graphicsengine)
include (dirs.graphicsengineshaders)
include (dirs.assetmanager)
include (dirs.imgui)
include (dirs.logger)
include (dirs.projectcreator)
include (dirs.networkengine)

include (dirs.network .. "NetworkShared")

include (dirs.application .. "ModelViewer")
include (dirs.application .. "FeatureShowcase")
include(dirs.application .. "Navigation")
include(dirs.application .. "Movement1")
include(dirs.application .. "Movement2")
include(dirs.application .. "DecisionMaking")
include(dirs.application .. "NetworkClient")
include(dirs.application .. "NetworkServer")