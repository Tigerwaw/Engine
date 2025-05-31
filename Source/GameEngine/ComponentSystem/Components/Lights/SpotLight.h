#pragma once
#include "ComponentSystem/Components/Lights/LightSource.h"
#include "Math/Vector.hpp"


class SpotLight : public LightSource
{
public:
    SpotLight(float aConeAngle = 45.0f, float aIntensity = 1.0f, Math::Vector3f aColor = { 1.0f, 1.0f, 1.0f });
    float GetConeAngleRadians() const { return myConeAngleRadians; }
    float GetConeAngleDegrees() const { return myConeAngleDegrees; }
    void SetConeAngle(float aConeAngle);

    bool Serialize(nl::json& outJsonObject) override;
    bool Deserialize(nl::json& aJsonObject) override;
protected:
    float myConeAngleRadians;
    float myConeAngleDegrees;
};

