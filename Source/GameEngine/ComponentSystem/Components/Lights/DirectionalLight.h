#pragma once
#include <memory>

#include "GameEngine/ComponentSystem/Components/Lights/LightSource.h"
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

class DirectionalLight : public LightSource
{
public:
	DirectionalLight(float aIntensity = 1.0f, CU::Vector3f aColor = { 1.0f, 1.0f, 1.0f });

private:
	bool myCastsShadows = false;
};

