#include "DirectionalLight.h"
#include "GameEngine/ComponentSystem/GameObject.h"

DirectionalLight::DirectionalLight(float aIntensity, CU::Vector3f aColor) : LightSource(aIntensity, aColor)
{
}
