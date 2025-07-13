#include "GraphicsEngine.pch.h"
#include "Material.h"
#include "GraphicsEngine.h"
#include "Objects/Texture.h"

Material::Material()
{
	MaterialSettings().albedoTint = { 1.0f, 1.0f, 1.0f, 1.0f };
	MaterialSettings().emissiveStrength = 0.0f;
	SetTexture(Material::TextureType::Albedo, GraphicsEngine::Get().GetDefaultAlbedoTexture());
	SetTexture(Material::TextureType::Normal, GraphicsEngine::Get().GetDefaultNormalTexture());
	SetTexture(Material::TextureType::Material, GraphicsEngine::Get().GetDefaultARMTexture());
	SetTexture(Material::TextureType::Effects, GraphicsEngine::Get().GetDefaultFXTexture());
}

std::shared_ptr<Material> Material::CreateInstance() const
{
	std::shared_ptr<Material> instance = std::make_shared<Material>();
	instance->myPSO = myPSO;
	instance->myMaterialSettings = myMaterialSettings;

	for (auto& [slot, texture] : myTextures)
	{
		instance->SetTextureOnSlot(slot, texture);
	}

	return instance;
}

void Material::SetTexture(TextureType aTextureType, std::shared_ptr<Texture> aTexture)
{
	SetTextureOnSlot(static_cast<unsigned>(aTextureType), aTexture);
}

Texture& Material::GetTexture(TextureType aTextureType)
{
	return GetTextureOnSlot(static_cast<unsigned>(aTextureType));
}

void Material::SetTextureOnSlot(unsigned aTextureSlot, std::shared_ptr<Texture> aTexture)
{
	myTextures[aTextureSlot] = aTexture;
}

Texture& Material::GetTextureOnSlot(unsigned aTextureSlot)
{
	if (myTextures.find(aTextureSlot) == myTextures.end())
	{
		assert("Can't find texture!");
	}

	return *myTextures.at(aTextureSlot);
}

void Material::ClearTextureOnSlot(unsigned aTextureSlot)
{
	if (myTextures.find(aTextureSlot) == myTextures.end())
	{
		assert("Can't find texture!");
	}

	myTextures.erase(myTextures.find(aTextureSlot));
}
