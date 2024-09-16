#include "GraphicsEngine.pch.h"
#include "Material.h"
#include "GraphicsEngine/GraphicsEngine.h"
#include "GraphicsEngine/Objects/Texture.h"

Material::Material()
{
}

std::shared_ptr<Material> Material::CreateInstance() const
{
	std::shared_ptr<Material> instance = std::make_shared<Material>();
	instance->myPSO = myPSO;
	instance->myMaterialSettings = myMaterialSettings;

	for (auto& texture : myTextures)
	{
		instance->SetTexture(texture.first, texture.second);
	}

	return instance;
}

void Material::SetTexture(TextureType aTextureType, std::shared_ptr<Texture> aTexture)
{
	myTextures[aTextureType] = aTexture;
}

Texture& Material::GetTexture(TextureType aTextureType)
{
	if (myTextures.find(aTextureType) == myTextures.begin())
	{
		assert("Can't find texture!");
	}

	return *myTextures.at(aTextureType);
}