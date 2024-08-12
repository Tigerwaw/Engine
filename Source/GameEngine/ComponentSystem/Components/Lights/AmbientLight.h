#pragma once
#include "GameEngine/ComponentSystem/Component.h"
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

class Texture;

class AmbientLight : public Component
{
public:
	AmbientLight(std::shared_ptr<Texture> aEnvironmentTexture, CU::Vector3f aColor = {1.0f, 1.0f, 1.0f}, float aIntensity = 1.0f);
	~AmbientLight() override;
	void Start() override;
	void Update() override;
	void SetColor(CU::Vector3f aColor);
	void SetIntensity(float aIntensity);
	std::shared_ptr<Texture> GetEnvironmentTexture() const;
	CU::Vector3f GetColor() const { return myColor; }
	float GetIntensity() const { return myIntensity; }
private:
	std::shared_ptr<Texture> myEnvironmentTexture;
	CU::Vector3f myColor = { 1.0f, 1.0f, 1.0f };
	float myIntensity = 1.0f;
};

