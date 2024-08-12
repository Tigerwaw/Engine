#include "Enginepch.h"

#include "AmbientLight.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Graphics/GraphicsEngine/Objects/Texture.h"

AmbientLight::AmbientLight(std::shared_ptr<Texture> aCubemap, CU::Vector3f aColor, float aIntensity)
{
	myCubemap = aCubemap;
	myColor = aColor;
	myIntensity = aIntensity;
}

AmbientLight::~AmbientLight()
{
	myCubemap = nullptr;
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

void AmbientLight::SetCubemap(std::shared_ptr<Texture> aCubemap)
{
	myCubemap = aCubemap;
}

std::shared_ptr<Texture> AmbientLight::GetCubemap() const
{
	return myCubemap;
}

bool AmbientLight::Serialize(nl::json& outJsonObject)
{
	outJsonObject;
	return false;
}

bool AmbientLight::Deserialize(nl::json& aJsonObject)
{
	if (aJsonObject.contains("Cubemap"))
	{
		SetCubemap(AssetManager::Get().GetAsset<TextureAsset>(aJsonObject["Cubemap"].get<std::string>())->texture);
	}

	return true;
}
