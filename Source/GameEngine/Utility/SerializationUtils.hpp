#pragma once
#include "Utilities/nlohmann/json.hpp"
#include "GameEngine/Math/Vector.hpp"
namespace nl = nlohmann;
namespace CU = CommonUtilities;

namespace Utility
{
    static CU::Vector3f DeserializeVector3(nl::json& aJsonObject)
    {
        CU::Vector3f vector;

        if (!aJsonObject.is_array()) return vector;
        if (aJsonObject.size() == 0) return vector;

        if (aJsonObject.size() > 0)
        {
            vector.x = aJsonObject[0];
        }

        if (aJsonObject.size() > 1)
        {
            vector.y = aJsonObject[1];
        }

        if (aJsonObject.size() > 2)
        {
            vector.z = aJsonObject[2];
        }

        return vector;
    }
}