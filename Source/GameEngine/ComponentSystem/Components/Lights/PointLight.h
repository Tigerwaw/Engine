#pragma once
#include "ComponentSystem/Components/Lights/LightSource.h"
#include "Math/Vector.hpp"


class PointLight : public LightSource
{
public:
	PointLight(float aIntensity = 1.0f, Math::Vector3f aColor = { 1.0f, 1.0f, 1.0f });
	void EnableShadowCasting(unsigned aShadowMapWidth, unsigned aShadowMapHeight) override;

	bool Serialize(nl::json& outJsonObject) override;
	bool Deserialize(nl::json& aJsonObject) override;
};

