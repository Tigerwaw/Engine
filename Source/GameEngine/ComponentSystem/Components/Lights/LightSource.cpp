#include "LightSource.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Graphics/GraphicsEngine/RHI/Texture.h"

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
    myCastsShadows = GraphicsEngine::Get().CreateShadowMap(myParent->GetName() + "_ShadowMap", aShadowMapWidth, aShadowMapHeight, *myShadowMap);
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
    return myParent->Transform.GetForwardVector();
}

CU::Vector3f LightSource::GetPosition() const
{
    return myParent->Transform.GetTranslation();
}

std::shared_ptr<Texture> LightSource::GetShadowMap()
{
    return myShadowMap;
}
