#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix.hpp"



class Camera;

class UpdateFrameBuffer : public GraphicsCommandBase
{
public:
    UpdateFrameBuffer(std::shared_ptr<Camera> aCamera);
    void Execute() override;
    void Destroy() override;
private:
    Math::Matrix4x4f myCameraInvView;
    Math::Matrix4x4f myCameraProj;
    Math::Vector4f myViewPosition;
    Math::Vector4f myViewDirection;
    float myTotalTime;
    float myDeltaTime;
    Math::Vector2f myResolution;
    float myNearPlane;
    float myFarPlane;
};

