#include "PointLight.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Graphics/GraphicsEngine/RHI/Texture.h"

PointLight::PointLight(float aIntensity, CU::Vector3f aColor) : LightSource(aIntensity, aColor)
{
}

void PointLight::EnableShadowCasting(unsigned aShadowMapWidth, unsigned aShadowMapHeight)
{
	myShadowMap = std::make_shared<Texture>();
	myCastsShadows = GraphicsEngine::Get().CreateShadowCubemap(myParent->GetName() + "_ShadowCubemap", aShadowMapWidth, aShadowMapHeight, *myShadowMap);
}