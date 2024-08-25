#include "Enginepch.h"

#include "DirectionalLight.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/Camera.h"
#include "GameEngine/Utility/SerializationUtils.hpp"

DirectionalLight::DirectionalLight(float aIntensity, CU::Vector3f aColor) : LightSource(aIntensity, aColor)
{
}

void DirectionalLight::RecalculateShadowFrustum(std::shared_ptr<GameObject> aRenderCamera, CU::AABB3D<float> aSceneBB)
{
    if (!myCastsShadows) return;
    std::shared_ptr<Camera> lightCam = gameObject->GetComponent<Camera>();
    if (!lightCam) return;
    std::shared_ptr<Transform> lightTransform = gameObject->GetComponent<Transform>();
    if (!lightTransform) return;
    std::shared_ptr<Camera> renderCam = aRenderCamera->GetComponent<Camera>();
    if (!renderCam) return;
    std::shared_ptr<Transform> camTransform = aRenderCamera->GetComponent<Transform>();
    if (!camTransform) return;

    CU::Matrix4x4f camToWorldMatrix = camTransform->GetWorldMatrix();
    CU::Matrix4x4f worldToLightMatrix = lightTransform->GetWorldMatrix().GetFastInverse();

    CU::Vector3f min = { FLT_MAX, FLT_MAX, FLT_MAX };
    CU::Vector3f max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    //for (CU::Vector3f corner : renderCam->GetFrustumCorners())
    //{
    //    corner = CU::ToVector3(CU::ToVector4(corner, 1.0f) * camToWorldMatrix);
    //    corner = CU::ToVector3(CU::ToVector4(corner, 1.0f) * worldToLightMatrix);

    //    min.x = std::fminf(corner.x, min.x);
    //    max.x = std::fmaxf(corner.x, max.x);
    //    min.y = std::fminf(corner.y, min.y);
    //    max.y = std::fmaxf(corner.y, max.y);
    //}

    for (CU::Vector3f corner : aSceneBB.GetCorners())
    {
        corner = CU::ToVector3(CU::ToVector4(corner, 1.0f) * worldToLightMatrix);

        min.x = std::fminf(corner.x, min.x);
        max.x = std::fmaxf(corner.x, max.x);
        min.y = std::fminf(corner.y, min.y);
        max.y = std::fmaxf(corner.y, max.y);
        min.z = std::fminf(corner.z, min.z);
        max.z = std::fmaxf(corner.z, max.z);
    }

    CU::Vector2f worldUnitsPerTexel = CU::ToVector2(max - min);
    worldUnitsPerTexel.x /= myShadowMapSize.x;
    worldUnitsPerTexel.y /= myShadowMapSize.y;

    min.x = floorf(min.x / worldUnitsPerTexel.x) * worldUnitsPerTexel.x;
    max.x = floorf(max.x / worldUnitsPerTexel.x) * worldUnitsPerTexel.x;

    min.y = floorf(min.y / worldUnitsPerTexel.y) * worldUnitsPerTexel.y;
    max.y = floorf(max.y / worldUnitsPerTexel.y) * worldUnitsPerTexel.y;

    lightCam->InitOrtographicProjection(min.x, max.x, max.y, min.y, min.z, max.z);
}

bool DirectionalLight::Serialize(nl::json& outJsonObject)
{
    outJsonObject;
    return false;
}

bool DirectionalLight::Deserialize(nl::json& aJsonObject)
{
    if (aJsonObject.contains("Intensity"))
    {
        SetIntensity(aJsonObject["Intensity"].get<float>());
    }

    if (aJsonObject.contains("Color"))
    {
        SetColor(Utility::DeserializeVector3<float>(aJsonObject["Color"]));
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
