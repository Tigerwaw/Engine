#pragma once
#include "GameEngine/ComponentSystem/Component.h"
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

class Texture;

class LightSource : public Component
{
public:
	LightSource(float aIntensity = 1.0f, CU::Vector3f aColor = { 1.0f, 1.0f, 1.0f });
	~LightSource() override;
	void Start() override;
	void Update() override;
	virtual void EnableShadowCasting(unsigned aShadowMapWidth, unsigned aShadowMapHeight);

	void SetColor(CU::Vector3f aColor);
	void SetIntensity(float aIntensity);
	CU::Vector3f GetColor() const { return myColor; }
	float GetIntensity() const { return myIntensity; }
	CU::Vector3f GetDirection() const;
	CU::Vector3f GetPosition() const;
	const bool CastsShadows() const { return myCastsShadows; }
	void SetShadowBias(float aMinShadowBias, float aMaxShadowBias);
	const float GetMinShadowBias() const { return myMinShadowBias; }
	const float GetMaxShadowBias() const { return myMaxShadowBias; }
	void SetShadowSamples(unsigned aNumSamples);
	const int GetShadowSamples() const { return myShadowSamples; }
	std::shared_ptr<Texture> GetShadowMap();
	const CU::Vector2f GetShadowMapSize() const { return myShadowMapSize; }
protected:
	CU::Vector3f myColor = { 1.0f, 1.0f, 1.0f };
	float myIntensity = 1.0f;
	bool myCastsShadows = false;
	std::shared_ptr<Texture> myShadowMap = nullptr;
	CU::Vector2f myShadowMapSize;
	float myMinShadowBias = 0.001f;
	float myMaxShadowBias = 0.005f;
	int myShadowSamples = 1;
};

