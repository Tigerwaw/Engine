#pragma once
#include "ComponentSystem/Component.h"
#include "Math/Vector.hpp"


class Texture;

class LightSource : public Component
{
public:
	LightSource(float aIntensity = 1.0f, Math::Vector3f aColor = { 1.0f, 1.0f, 1.0f });
	~LightSource() override;
	void Start() override;
	void Update() override;
	virtual void EnableShadowCasting(unsigned aShadowMapWidth, unsigned aShadowMapHeight);

	void SetColor(Math::Vector3f aColor);
	void SetIntensity(float aIntensity);
	Math::Vector3f GetColor() const { return myColor; }
	float GetIntensity() const { return myIntensity; }
	Math::Vector3f GetDirection() const;
	Math::Vector3f GetPosition() const;
	const bool CastsShadows() const { return myCastsShadows; }
	void SetShadowBias(float aMinShadowBias, float aMaxShadowBias);
	const float GetMinShadowBias() const { return myMinShadowBias; }
	const float GetMaxShadowBias() const { return myMaxShadowBias; }
	void SetLightSize(float aSize);
	const float GetLightSize() const { return myLightSize; }
	std::shared_ptr<Texture> GetShadowMap();
	const Math::Vector2f GetShadowMapSize() const { return myShadowMapSize; }
protected:
	Math::Vector3f myColor = { 1.0f, 1.0f, 1.0f };
	float myIntensity = 1.0f;
	bool myCastsShadows = false;
	std::shared_ptr<Texture> myShadowMap = nullptr;
	Math::Vector2f myShadowMapSize;
	float myMinShadowBias = 0.001f;
	float myMaxShadowBias = 0.005f;
	float myLightSize = 1.0f;
};

