#include "LightSource.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Graphics/GraphicsEngine/Objects/Texture.h"

LightSource::LightSource(float aIntensity, CU::Vector3f aColor)
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
    myCastsShadows = GraphicsEngine::Get().CreateShadowMap(gameObject->GetName() + "_ShadowMap", aShadowMapWidth, aShadowMapHeight, *myShadowMap);
}

void LightSource::SetColor(CU::Vector3f aColor)
{
    myColor = aColor;
}

void LightSource::SetIntensity(float aIntensity)
{
    myIntensity = aIntensity;
}

CU::Vector3f LightSource::GetDirection() const
{
    return gameObject->GetComponent<Transform>()->GetForwardVector(true);
}

CU::Vector3f LightSource::GetPosition() const
{
    return gameObject->GetComponent<Transform>()->GetTranslation(true);
}

void LightSource::SetShadowBias(float aMinShadowBias, float aMaxShadowBias)
{
    myMinShadowBias = aMinShadowBias;
    myMaxShadowBias = aMaxShadowBias;
}

void LightSource::SetShadowSamples(unsigned aNumSamples)
{
    myShadowSamples = static_cast<int>(aNumSamples);
}

std::shared_ptr<Texture> LightSource::GetShadowMap()
{
    return myShadowMap;
}
