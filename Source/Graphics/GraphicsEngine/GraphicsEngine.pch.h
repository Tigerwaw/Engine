#pragma once
#include <d3d11.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <wrl.h>

#include <memory>
#include <cstdint>
#include <cstring>
#include <functional>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <random>

#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <any>

#include <nlohmann/json.hpp>
namespace nl = nlohmann;

#ifndef _RETAIL
#include "Logger/Logger.h"
#define USE_PIX
#endif
#include "WinPixEventRuntime/pix3.h"

#if _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(LogGraphicsEngine, GraphicsEngine, Verbose);
DECLARE_LOG_CATEGORY_WITH_NAME(LogRHI, RHI, Verbose);
#elif _RELEASE
DECLARE_LOG_CATEGORY_WITH_NAME(LogGraphicsEngine, GraphicsEngine, Warning);
DECLARE_LOG_CATEGORY_WITH_NAME(LogRHI, RHI, Warning);
#else
#define LOG(Category, Verbosity, Message, ...);
#endif

#include "GraphicsEngine.h"