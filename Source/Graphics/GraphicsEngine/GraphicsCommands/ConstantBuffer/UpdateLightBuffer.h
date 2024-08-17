#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"

class AmbientLight;
class DirectionalLight;
class PointLight;
class SpotLight;

struct UpdateLightBuffer : public GraphicsCommandBase
{
public:
    UpdateLightBuffer(std::shared_ptr<AmbientLight> aAmbientLight, std::shared_ptr<DirectionalLight> aDirLight, std::vector<std::shared_ptr<PointLight>> aPointLights, std::vector<std::shared_ptr<SpotLight>> aSpotLights);
    void Execute() override;
    void Destroy() override;
private:
    std::shared_ptr<AmbientLight> myAmbientLight;
    std::shared_ptr<DirectionalLight> myDirectionalLight;
    std::vector<std::shared_ptr<PointLight>> myPointLights;
    std::vector<std::shared_ptr<SpotLight>> mySpotLights;
};

