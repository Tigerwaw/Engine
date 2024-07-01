#pragma once
#include "Graphics/GraphicsEngine/GraphicsCommands/GraphicsCommandBase.h"
#include <memory>
#include "GameEngine/Math/Matrix.hpp"

namespace CU = CommonUtilities;

class Camera;

class UpdateFrameBuffer : public GraphicsCommandBase
{
public:
    UpdateFrameBuffer(std::shared_ptr<Camera> aCamera);
    void Execute() override;
    void Destroy() override;
private:
    CU::Matrix4x4f myCameraInvView;
    CU::Matrix4x4f myCameraProj;
    CU::Vector4f myViewPosition;
    CU::Vector4f myViewDirection;
    float myTotalTime;
    float myDeltaTime;
    CU::Vector2f myResolution;
    float myNearPlane;
    float myFarPlane;
};

