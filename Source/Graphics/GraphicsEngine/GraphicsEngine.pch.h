// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

// add headers that you want to pre-compile here
#include "Logger/Logger.h"
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

#include "GraphicsEngine/GraphicsEngine.h"

#if _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(LogGraphicsEngine, GraphicsEngine, Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(LogGraphicsEngine, GraphicsEngine, Warning);
#endif

#define GELOG(Verbosity, Message, ...) LOG(LogGraphicsEngine, Verbosity, Message, ##__VA_ARGS__)
