#include "Enginepch.h"

#include "LightSource.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"
#include "GraphicsEngine.h"
#include "Objects/Texture.h"

LightSource::LightSource(float aIntensity, Math::Vector3f aColor)
{
    SetColor(aColor);
    SetIntensity(aIntensity);
}

LightSource::~LightSource()
{
    myShadowMap = nullptr;
}

void LightSource::Start()
{
}

void LightSource::Update()
{
}

void LightSource::EnableShadowCasting(unsigned aShadowMapWidth, unsigned aShadowMapHeight)
{
    myShadowMap = std::make_shared<Texture>();
    myShadowMapSize = { static_cast<float>(aShadowMapWidth), static_cast<float>(aShadowMapHeight) };
    myCastsShadows = GraphicsEngine::Get().GetResourceVendor().CreateShadowMap(gameObject->GetName() + "_ShadowMap", aShadowMapWidth, aShadowMapHeight, *myShadowMap);
}

void LightSource::SetColor(Math::Vector3f aColor)
{
    myColor = aColor;
}

void LightSource::SetIntensity(float aIntensity)
{
    myIntensity = aIntensity;
}

Math::Vector3f LightSource::GetDirection() const
{
    return gameObject->GetComponent<Transform>()->GetForwardVector(true);
}

Math::Vector3f LightSource::GetPosition() const
{
    return gameObject->GetComponent<Transform>()->GetTranslation(true);
}

void LightSource::SetShadowBias(float aMinShadowBias, float aMaxShadowBias)
{
    myMinShadowBias = aMinShadowBias;
    myMaxShadowBias = aMaxShadowBias;
}

void LightSource::SetLightSize(float aSize)
{
    myLightSize = aSize;
}

std::shared_ptr<Texture> LightSource::GetShadowMap()
{
    return myShadowMap;
}
