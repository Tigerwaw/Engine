#pragma once
#include "Utilities/nlohmann/json.hpp"
namespace nl = nlohmann;
#include "TGAFBX/TGAFbx.h"

#include <d3d11.h>
#include <wrl.h>

#include <memory>
#include <cstdint>
#include <cstring>
#include <functional>
#include <stdexcept>
#include <filesystem>
#include <iostream>
#include <fstream>

#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <any>


#include "Logger/Logger.h"

#ifdef _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(LogAssetManager, AssetManager, Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(LogAssetManager, AssetManager, Warning);
#endif