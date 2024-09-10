#include "GraphicsEngine.pch.h"
#include "Material.h"
#include "GraphicsEngine/GraphicsEngine.h"
#include "GraphicsEngine/Objects/Texture.h"

Material::Material()
{
	myAlbedoTexture = std::make_shared<Texture>();
	myNormalTexture = std::make_shared<Texture>();
	myMaterialTexture = std::make_shared<Texture>();
}

std::shared_ptr<Material> Material::CreateInstance() const
{
	std::shared_ptr<Material> instance = std::make_shared<Material>();
	instance->myPSO = myPSO;
	instance->myMaterialSettings = myMaterialSettings;
	instance->myAlbedoTexture = myAlbedoTexture;
	instance->myNormalTexture = myNormalTexture;
	instance->myMaterialTexture = myMaterialTexture;

	return instance;
}
