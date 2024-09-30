#pragma once
#include "Utilities/nlohmann/json.hpp"
#include "GameEngine/Math/Vector.hpp"
namespace nl = nlohmann;
namespace CU = CommonUtilities;

namespace Utility
{
    template <class T>
    static CU::Vector4<T> DeserializeVector4(nl::json& aJsonObject)
    {
        CU::Vector4<T> vector;

        if (!aJsonObject.is_array()) return vector;
        if (aJsonObject.size() == 0) return vector;

        if (aJsonObject.size() > 0)
        {
            vector.x = aJsonObject[0].get<T>();
        }

        if (aJsonObject.size() > 1)
        {
            vector.y = aJsonObject[1].get<T>();
        }

        if (aJsonObject.size() > 2)
        {
            vector.z = aJsonObject[2].get<T>();
        }

        if (aJsonObject.size() > 3)
        {
            vector.w = aJsonObject[3].get<T>();
        }

        return vector;
    }

    template <class T>
    static CU::Vector3<T> DeserializeVector3(nl::json& aJsonObject)
    {
        CU::Vector3<T> vector;

        if (!aJsonObject.is_array()) return vector;
        if (aJsonObject.size() == 0) return vector;

        if (aJsonObject.size() > 0)
        {
            vector.x = aJsonObject[0].get<T>();
        }

        if (aJsonObject.size() > 1)
        {
            vector.y = aJsonObject[1].get<T>();
        }

        if (aJsonObject.size() > 2)
        {
            vector.z = aJsonObject[2].get<T>();
        }

        return vector;
    }

    template <class T>
    static CU::Vector2<T> DeserializeVector2(nl::json& aJsonObject)
    {
        CU::Vector2<T> vector;

        if (!aJsonObject.is_array()) return vector;
        if (aJsonObject.size() == 0) return vector;

        if (aJsonObject.size() > 0)
        {
            vector.x = aJsonObject[0].get<T>();
        }

        if (aJsonObject.size() > 1)
        {
            vector.y = aJsonObject[1].get<T>();
        }

        return vector;
    }
}