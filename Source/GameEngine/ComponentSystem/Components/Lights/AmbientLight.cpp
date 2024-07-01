#include "AmbientLight.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Graphics/GraphicsEngine/Objects/Texture.h"

AmbientLight::AmbientLight(std::shared_ptr<Texture> aEnvironmentTexture, CU::Vector3f aColor, float aIntensity)
{
	myEnvironmentTexture = aEnvironmentTexture;
	myColor = aColor;
	myIntensity = aIntensity;
}

AmbientLight::~AmbientLight()
{
	myEnvironmentTexture = nullptr;
}

void AmbientLight::Start()
{
}

void AmbientLight::Update()
{
}

void AmbientLight::SetColor(CU::Vector3f aColor)
{
	myColor = aColor;
}

void AmbientLight::SetIntensity(float aIntensity)
{
	myIntensity = aIntensity;
}

std::shared_ptr<Texture> AmbientLight::GetEnvironmentTexture() const
{
	return myEnvironmentTexture;
}
