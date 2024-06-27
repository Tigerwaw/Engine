#pragma once
#include "GameEngine/ComponentSystem/Components/Lights/LightSource.h"
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

class SpotLight : public LightSource
{
public:
    SpotLight(float aConeAngle = 45.0f, float aIntensity = 1.0f, CU::Vector3f aColor = { 1.0f, 1.0f, 1.0f });
    float GetConeAngleRadians() const { return myConeAngleRadians; }
    float GetConeAngleDegrees() const { return myConeAngleDegrees; }
    void SetConeAngle(float aConeAngle);
protected:
    float myConeAngleRadians;
    float myConeAngleDegrees;
};

