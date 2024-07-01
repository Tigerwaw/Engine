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
