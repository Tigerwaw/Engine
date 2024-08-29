include "common.lua"

workspace "TGPGameEngine"
  location "%{dirs.root}"
  architecture "x64"
  configurations { "Debug", "Internal", "Release" }
  startproject "ModelViewer"

include (dirs.engine)
include (dirs.graphicsengine)
include (dirs.graphicsengineshaders)
include (dirs.assetmanager)
include (dirs.imgui)
include (dirs.logger)
include (dirs.projectcreator)
include (dirs.application .. "ModelViewer")
include (dirs.application .. "FeatureShowcase")