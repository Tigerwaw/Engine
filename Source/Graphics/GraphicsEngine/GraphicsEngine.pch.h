#pragma once
#include <d3d11.h>
#include <wrl.h>

#include <memory>
#include <cstdint>
#include <cstring>
#include <functional>
#include <stdexcept>
#include <filesystem>

#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <any>

#include <nlohmann/json.hpp>
namespace nl = nlohmann;

#include "Logger/Logger.h"
#include "GraphicsEngine/GraphicsEngine.h"

#if _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(LogGraphicsEngine, GraphicsEngine, Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(LogGraphicsEngine, GraphicsEngine, Warning);
#endif

#define GELOG(Verbosity, Message, ...) LOG(LogGraphicsEngine, Verbosity, Message, ##__VA_ARGS__)
