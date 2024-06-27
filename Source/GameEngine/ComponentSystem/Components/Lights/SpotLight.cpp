#include "SpotLight.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/Math/MathConstants.hpp"

SpotLight::SpotLight(float aConeAngle, float aIntensity, CU::Vector3f aColor) : LightSource(aIntensity, aColor)
{
    SetConeAngle(aConeAngle);
}

void SpotLight::SetConeAngle(float aConeAngle)
{
    myConeAngleDegrees = aConeAngle;
    myConeAngleRadians = aConeAngle * CU::DEGREES_TO_RADIANS;
}
