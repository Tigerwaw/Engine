#include "Enginepch.h"

#include "PointLight.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Graphics/GraphicsEngine/Objects/Texture.h"
#include "GameEngine/Utility/SerializationUtils.hpp"

PointLight::PointLight(float aIntensity, CU::Vector3f aColor) : LightSource(aIntensity, aColor)
{
}

void PointLight::EnableShadowCasting(unsigned aShadowMapWidth, unsigned aShadowMapHeight)
{
	myShadowMap = std::make_shared<Texture>();
	myCastsShadows = GraphicsEngine::Get().CreateShadowCubemap(gameObject->GetName() + "_ShadowCubemap", aShadowMapWidth, aShadowMapHeight, *myShadowMap);
}

bool PointLight::Serialize(nl::json& outJsonObject)
{
    outJsonObject;
	return false;
}

bool PointLight::Deserialize(nl::json& aJsonObject)
{
    if (aJsonObject.contains("Intensity"))
    {
        SetIntensity(aJsonObject["Intensity"].get<float>());
    }

    if (aJsonObject.contains("Color"))
    {
        SetColor(Utility::DeserializeVector3(aJsonObject["Color"]));
    }

    if (aJsonObject.contains("CastShadows") && aJsonObject["CastShadows"].get<bool>())
    {
        unsigned shadowTextureSize = 512;
        float minShadowBias = 0.001f;
        float maxShadowBias = 0.005f;
        unsigned shadowSamples = 1;

        if (aJsonObject.contains("ShadowTextureSize"))
        {
            shadowTextureSize = aJsonObject["ShadowTextureSize"].get<unsigned>();
        }

        if (aJsonObject.contains("MinShadowBias"))
        {
            minShadowBias = aJsonObject["MinShadowBias"].get<float>();
        }

        if (aJsonObject.contains("MaxShadowBias"))
        {
            maxShadowBias = aJsonObject["MaxShadowBias"].get<float>();
        }

        if (aJsonObject.contains("ShadowSamples"))
        {
            shadowSamples = aJsonObject["ShadowSamples"].get<unsigned>();
        }

        EnableShadowCasting(shadowTextureSize, shadowTextureSize);
        SetShadowBias(minShadowBias, maxShadowBias);
        SetShadowSamples(shadowSamples);
    }

	return true;
}
