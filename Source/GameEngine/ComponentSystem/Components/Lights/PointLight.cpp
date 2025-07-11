#include "Enginepch.h"

#include "PointLight.h"
#include "ComponentSystem/GameObject.h"
#include "GraphicsEngine.h"
#include "Objects/Texture.h"
#include "CommonUtilities/SerializationUtils.hpp"

PointLight::PointLight(float aIntensity, Math::Vector3f aColor) : LightSource(aIntensity, aColor)
{
}

void PointLight::EnableShadowCasting(unsigned aShadowMapWidth, unsigned aShadowMapHeight)
{
	myShadowMap = std::make_shared<Texture>();
	myCastsShadows = GraphicsEngine::Get().GetResourceVendor().CreateShadowCubemap(gameObject->GetName() + "_ShadowCubemap", aShadowMapWidth, aShadowMapHeight, *myShadowMap);
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
        SetColor(Utilities::DeserializeVector3<float>(aJsonObject["Color"]));
    }

    if (aJsonObject.contains("CastShadows") && aJsonObject["CastShadows"].get<bool>())
    {
        unsigned shadowTextureSize = 512;
        float minShadowBias = 0.001f;
        float maxShadowBias = 0.005f;
        float lightSize = 1.0f;

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

        if (aJsonObject.contains("LightSize"))
        {
            lightSize = aJsonObject["LightSize"].get<float>();
        }

        EnableShadowCasting(shadowTextureSize, shadowTextureSize);
        SetShadowBias(minShadowBias, maxShadowBias);
        SetLightSize(lightSize);
    }

	return true;
}
