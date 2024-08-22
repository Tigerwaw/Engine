#pragma once

class GameObject;
class Scene;
class Camera;
struct PipelineStateObject;

class Renderer
{
public:
    Renderer();
    ~Renderer();
    void RenderScene(Scene& aScene);
private:
    void QueueClearTextureResources();
    void QueueShadowmapTextureResources(Scene& aScene);
    void QueueUpdateLightBuffer(Scene& aScene);
    void QueueSpotLightShadows(Scene& aScene);
    void QueuePointLightShadows(Scene& aScene);
    void QueueDirectionalLightShadows(Scene& aScene);
    void QueueDebugGizmos(Scene& aScene, std::shared_ptr<Camera> aRenderCamera);
    void QueueGameObjects(Scene& aScene, std::shared_ptr<Camera> aRenderCamera, bool disableViewCulling = false, std::shared_ptr<PipelineStateObject> aPSOoverride = nullptr);
};

