#pragma once
#include <memory>

#include "GameEngine/ComponentSystem/Components/Lights/LightSource.h"
#include "GameEngine/Math/Vector.hpp"
#include "GameEngine/Intersections/AABB3D.hpp"
namespace CU = CommonUtilities;

class GameObject;

class DirectionalLight : public LightSource
{
public:
	DirectionalLight(float aIntensity = 1.0f, CU::Vector3f aColor = { 1.0f, 1.0f, 1.0f });
	void RecalculateShadowFrustum(std::shared_ptr<GameObject> aRenderCamera, CU::AABB3D<float> aSceneBB);
};

