#pragma once
#include <windows.h>
#include <Windowsx.h>
#include <wrl.h>

#include <memory>
#include <functional>
#include <algorithm>
#include <math.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
namespace nl = nlohmann;

#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <any>

#include "AssetManager/AssetManager.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Audio/AudioEngine.h"

#include "utilities/Imgui/imgui.h"
#include "utilities/Imgui/backends/imgui_impl_win32.h"
#include "utilities/Imgui/backends/imgui_impl_dx11.h"
#include "fmod/fmod.hpp"
#include "fmod/fmod_studio.hpp"
#include "fmod/fmod_common.h"

#include "Logger/Logger.h"

#ifdef _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(LogGameEngine, GameEngine, Verbose);
DECLARE_LOG_CATEGORY_WITH_NAME(LogApplication, Application, Verbose);
DECLARE_LOG_CATEGORY_WITH_NAME(LogAudioEngine, AudioEngine, Verbose);
DECLARE_LOG_CATEGORY_WITH_NAME(LogAudioPlayer, AudioInstance, Verbose);
DECLARE_LOG_CATEGORY_WITH_NAME(LogDebugDrawer, DebugDrawer, Verbose);
DECLARE_LOG_CATEGORY_WITH_NAME(LogScene, Scene, Verbose);
DECLARE_LOG_CATEGORY_WITH_NAME(LogSceneHandler, SceneHandler, Verbose);
DECLARE_LOG_CATEGORY_WITH_NAME(LogSceneLoader, SceneLoader, Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(LogGameEngine, GameEngine, Warning);
DECLARE_LOG_CATEGORY_WITH_NAME(LogApplication, Application, Warning);
DECLARE_LOG_CATEGORY_WITH_NAME(LogAudioEngine, AudioEngine, Warning);
DECLARE_LOG_CATEGORY_WITH_NAME(LogAudioPlayer, AudioInstance, Warning);
DECLARE_LOG_CATEGORY_WITH_NAME(LogDebugDrawer, DebugDrawer, Warning);
DECLARE_LOG_CATEGORY_WITH_NAME(LogScene, Scene, Warning);
DECLARE_LOG_CATEGORY_WITH_NAME(LogSceneHandler, SceneHandler, Warning);
DECLARE_LOG_CATEGORY_WITH_NAME(LogSceneLoader, SceneLoader, Warning);
#endif