#pragma once
#include <vector>
#include <memory>
#include "GameEngine/Math/Matrix4x4.hpp"
#include "GameEngine/Math/Vector.hpp"
#include "GameEngine/Intersections/AABB3D.hpp"
namespace CU = CommonUtilities;

class Camera;
class Model;
class AnimatedModel;
class DebugModel;

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
    void DrawCameraFrustum(std::shared_ptr<Camera> aCamera, CU::Vector4f aColor = { 1.0f, 1.0f, 1.0f, 1.0f });
    void DrawBoundingBox(std::shared_ptr<Model> aModel, CU::Vector4f aColor = { 1.0f, 1.0f, 1.0f, 1.0f });
    void DrawBoundingBox(std::shared_ptr<AnimatedModel> aModel, CU::Vector4f aColor = { 1.0f, 1.0f, 1.0f, 1.0f });
    void DrawBoundingBox(std::shared_ptr<DebugModel> aModel, CU::Vector4f aColor = { 1.0f, 1.0f, 1.0f, 1.0f });
private:
    void DrawBoundingBoxInternal(CU::AABB3D<float> aAABB, CU::Matrix4x4f aWorldMatrix, CU::Vector4f aColor);

    std::vector<DebugLine> myLines;
};

