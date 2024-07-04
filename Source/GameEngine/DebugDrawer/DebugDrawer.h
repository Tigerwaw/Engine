#pragma once
#include <vector>
#include <memory>
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

class Camera;

struct DebugLine
{
    CU::Vector4f fromPosition;
    CU::Vector4f toPosition;
    CU::Vector4f color;
};

class DebugDrawer
{
public:
    void DrawObjects();
    void ClearObjects();
    void DrawLine(CU::Vector3f aFromPosition, CU::Vector3f aToPosition, CU::Vector4f aColor = { 1.0f, 1.0f, 1.0f, 1.0f });
    void DrawCameraFrustum(std::shared_ptr<Camera> aCamera);
private:

    std::vector<DebugLine> myLines;
};

