#pragma once
#include "Math/AABB3D.hpp"

class GameObject;
class Scene;
class Camera;
class Transform;
class AmbientLight;
class DirectionalLight;
class PointLight;
class SpotLight;
class Model;
class AnimatedModel;
class InstancedModel;
class ParticleSystem;
class TrailSystem;

// TEMP
class Sprite;
class Text;

class RenderAssembler
{
public:
    RenderAssembler();
    ~RenderAssembler();
    void RenderScene(Scene& aScene);

    // TEMP
    void Init();
private:
    struct SceneRenderData
    {
        Math::AABB3D<float> sceneBoundingBox;
        std::shared_ptr<Camera> mainCamera;
        std::shared_ptr<AmbientLight> ambientLight;
        std::shared_ptr<DirectionalLight> directionalLight;
        std::vector<std::shared_ptr<PointLight>> pointLights;
        std::vector<std::shared_ptr<SpotLight>> spotLights;

        std::vector<std::shared_ptr<GameObject>> castShadows;
        std::vector<std::shared_ptr<GameObject>> drawDeferred;
        std::vector<std::shared_ptr<GameObject>> drawForward;
        std::vector<std::shared_ptr<GameObject>> drawParticleSystems;

        std::vector<std::shared_ptr<GameObject>> drawCollidersObjects;
        std::vector<std::shared_ptr<GameObject>> drawBoundingBoxesObjects;
        std::vector<std::shared_ptr<GameObject>> drawCameraFrustumsObjects;
    };

    SceneRenderData AssembleLists(Scene& aScene);
    void SortRenderables(SceneRenderData& aRenderData);

    void RenderDebug(SceneRenderData& aRenderData);
    void RenderDeferred(SceneRenderData& aRenderData);

    void QueueDeferredObjects(SceneRenderData& aRenderData);
    void QueueForwardObjects(SceneRenderData& aRenderData);
    void QueueShadowmapTextureResources(SceneRenderData& aRenderData);
    void QueueUpdateLightBuffer(SceneRenderData& aRenderData);
    void QueueSpotLightShadows(SceneRenderData& aRenderData);
    void QueuePointLightShadows(SceneRenderData& aRenderData);
    void QueueDirectionalLightShadows(SceneRenderData& aRenderData);
    void QueueObjectShadows(SceneRenderData& aRenderData, std::shared_ptr<Camera> aRenderCamera);
    void QueueObjectShadows(SceneRenderData& aRenderData, std::shared_ptr<PointLight> aPointLight);
    void QueueObjectsDebug(SceneRenderData& aRenderData);
    void QueueDebugLines(SceneRenderData& aRenderData);

    bool IsInsideFrustum(std::shared_ptr<Camera> aRenderCamera, std::shared_ptr<Transform> aObjectTransform, const Math::AABB3D<float>& aObjectAABB);
    bool IsInsideRadius(std::shared_ptr<PointLight> aPointLight, std::shared_ptr<Transform> aObjectTransform, const Math::AABB3D<float>& aObjectAABB);

    void UpdateBoundingBox(std::shared_ptr<Transform> aTransform, const Math::AABB3D<float>& aBoundingBox);
    Math::AABB3D<float> myVisibleObjectsBB;

    // TEMP
    void DrawTestUI();
    std::shared_ptr<Sprite> myTestSprite;
    std::shared_ptr<Text> myTestText;
};

