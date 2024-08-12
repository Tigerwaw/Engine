#pragma once
#include "GameEngine/ComponentSystem/Components/Lights/LightSource.h"
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

class PointLight : public LightSource
{
public:
	PointLight(float aIntensity = 1.0f, CU::Vector3f aColor = { 1.0f, 1.0f, 1.0f });
	void EnableShadowCasting(unsigned aShadowMapWidth, unsigned aShadowMapHeight) override;

	bool Serialize(nl::json& outJsonObject) override;
	bool Deserialize(nl::json& aJsonObject) override;
};

