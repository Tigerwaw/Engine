#include "Enginepch.h"

#include "SpotLight.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/Math/MathConstants.hpp"
#include "GameEngine/Utility/SerializationUtils.hpp"

SpotLight::SpotLight(float aConeAngle, float aIntensity, CU::Vector3f aColor) : LightSource(aIntensity, aColor)
{
    SetConeAngle(aConeAngle);
}

void SpotLight::SetConeAngle(float aConeAngle)
{
    myConeAngleDegrees = aConeAngle;
    myConeAngleRadians = aConeAngle * CU::DEGREES_TO_RADIANS;
}

bool SpotLight::Serialize(nl::json& outJsonObject)
{
    outJsonObject;
    return false;
}

bool SpotLight::Deserialize(nl::json& aJsonObject)
{
    if (aJsonObject.contains("Intensity"))
    {
        SetIntensity(aJsonObject["Intensity"].get<float>());
    }

    if (aJsonObject.contains("Color"))
    {
        SetColor(Utility::DeserializeVector3<float>(aJsonObject["Color"]));
    }

    if (aJsonObject.contains("ConeAngle"))
    {
        SetConeAngle(aJsonObject["ConeAngle"].get<float>());
    }

    if (aJsonObject.contains("CastShadows") && aJsonObject["CastShadows"].get<bool>())
    {
        unsigned shadowTextureSize = 512;
        float minShadowBias = 0.001f;
        float maxShadowBias = 0.005f;
        float lightSize = 1.0f;

        if (aJsonObject.contains("ShadowTextureSize"))
        {
            shadowTextureSize = aJsonObject["ShadowTextureSize"].get<unsigned>();
        }

        if (aJsonObject.contains("MinShadowBias"))
        {
            minShadowBias = aJsonObject["MinShadowBias"].get<float>();
        }

        if (aJsonObject.contains("MaxShadowBias"))
        {
            maxShadowBias = aJsonObject["MaxShadowBias"].get<float>();
        }

        if (aJsonObject.contains("LightSize"))
        {
            lightSize = aJsonObject["LightSize"].get<float>();
        }

        EnableShadowCasting(shadowTextureSize, shadowTextureSize);
        SetShadowBias(minShadowBias, maxShadowBias);
        SetLightSize(lightSize);
    }

    return true;
}
