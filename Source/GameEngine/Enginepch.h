#pragma once
#include <windows.h>
#include <Windowsx.h>
#include <wrl.h>

#include <memory>
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

#include "Logger/Logger.h"
#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include "fmod/fmod.hpp"
#include "fmod/fmod_studio.hpp"
#include "fmod/fmod_common.h"