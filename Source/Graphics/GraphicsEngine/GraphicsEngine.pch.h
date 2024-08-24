#pragma once
#include <d3d11.h>
#include <wrl.h>

#include <memory>
#include <cstdint>
#include <cstring>
#include <functional>
#include <stdexcept>
#include <filesystem>
#include <fstream>

#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <any>

#include <nlohmann/json.hpp>
namespace nl = nlohmann;

#ifndef _RELEASE
#include "Logger/Logger.h"
#endif

#if _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(LogGraphicsEngine, GraphicsEngine, Verbose);
DECLARE_LOG_CATEGORY_WITH_NAME(LogRHI, RHI, Verbose);
#elif _INTERNAL
DECLARE_LOG_CATEGORY_WITH_NAME(LogGraphicsEngine, GraphicsEngine, Warning);
DECLARE_LOG_CATEGORY_WITH_NAME(LogRHI, RHI, Warning);
#else
#define LOG(Category, Verbosity, Message, ...);
#endif

#include "GraphicsEngine.h"