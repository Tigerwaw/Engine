#pragma once
#include "GameEngine/Intersections/AABB3D.hpp"
namespace CU = CommonUtilities;

class GameObject;
class Scene;
class Camera;
class Transform;
class PointLight;
struct PipelineStateObject;

// TEMP
class Sprite;
class Text;

class Renderer
{
public:
    Renderer();
    ~Renderer();
    void RenderScene(Scene& aScene);

    // TEMP
    void Init();
private:
    void RenderForward(Scene& aScene);
    void RenderDeferred(Scene& aScene);
    void RenderDeferredObjects(Scene& aScene, bool aDisableViewCulling = false);
    void RenderForwardObjects(Scene& aScene, bool aDisableViewCulling = false);

    void QueueShadowmapTextureResources(Scene& aScene);
    void QueueUpdateLightBuffer(Scene& aScene);
    void QueueSpotLightShadows(Scene& aScene);
    void QueuePointLightShadows(Scene& aScene);
    void QueueDirectionalLightShadows(Scene& aScene);
    void QueueDebugGizmos(Scene& aScene, std::shared_ptr<Camera> aRenderCamera);
    void QueueGameObjects(Scene& aScene, std::shared_ptr<Camera> aRenderCamera, bool aDisableViewCulling = false, std::shared_ptr<PipelineStateObject> aPSOoverride = nullptr);
    void QueueGameObjects(Scene& aScene, std::shared_ptr<PointLight> aPointLight, bool aDisableViewCulling = false, std::shared_ptr<PipelineStateObject> aPSOoverride = nullptr);
    void QueueDebugLines(Scene& aScene);

    bool IsInsideFrustum(std::shared_ptr<Camera> aRenderCamera, std::shared_ptr<Transform> aObjectTransform, CU::AABB3D<float> aObjectAABB);
    bool IsInsideRadius(std::shared_ptr<PointLight> aPointLight, std::shared_ptr<Transform> aObjectTransform, CU::AABB3D<float> aObjectAABB);

    void UpdateBoundingBox(std::shared_ptr<GameObject> aGameObject);
    CU::AABB3D<float> myVisibleObjectsBB;

    // TEMP
    void DrawTestUI();
    std::shared_ptr<Sprite> myTestSprite;
    std::shared_ptr<Text> myTestText;
};

