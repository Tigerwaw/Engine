#pragma once
#include "GameEngine/Math/Matrix4x4.hpp"
#include "GameEngine/Math/Vector.hpp"
#include "GameEngine/Intersections/AABB3D.hpp"
#include "GameEngine/Intersections/Sphere.hpp"
namespace CU = CommonUtilities;

#include "Graphics/GraphicsEngine/Objects/Vertices/DebugLineVertex.h"
#include "DebugDrawer/DebugLine.hpp"

class Camera;
class Model;
class AnimatedModel;
class DebugModel;

class DynamicVertexBuffer;

class DebugDrawer
{
public:
    void InitializeDebugDrawer();
    void DrawObjects();
    void ClearObjects();
    void DrawLine(CU::Vector3f aFromPosition, CU::Vector3f aToPosition, CU::Vector4f aColor = { 1.0f, 1.0f, 1.0f, 1.0f });
    void DrawLine(DebugLine aLine);
    void DrawCameraFrustum(std::shared_ptr<Camera> aCamera, CU::Vector4f aColor = { 1.0f, 1.0f, 1.0f, 1.0f });
    void DrawBoundingBox(CU::AABB3D<float> aAABB, CU::Matrix4x4f aWorldMatrix = CU::Matrix4x4f(), CU::Vector4f aColor = { 1.0f, 1.0f, 1.0f, 1.0f });
    void DrawBoundingBox(std::shared_ptr<Model> aModel, CU::Vector4f aColor = { 1.0f, 1.0f, 1.0f, 1.0f });
    void DrawBoundingBox(std::shared_ptr<AnimatedModel> aModel, CU::Vector4f aColor = { 1.0f, 1.0f, 1.0f, 1.0f });
    void DrawBoundingBox(std::shared_ptr<DebugModel> aModel, CU::Vector4f aColor = { 1.0f, 1.0f, 1.0f, 1.0f });
    void DrawBoundingSphere(CU::Sphere<float> aSphere, CU::Matrix4x4f aWorldMatrix = CU::Matrix4x4f(), CU::Vector4f aColor = { 1.0f, 1.0f, 1.0f, 1.0f });
private:

    std::vector<DebugLineVertex> myLineVertices;
    std::shared_ptr<DynamicVertexBuffer> myLineBuffer;

    bool myHasWarned = false;
};

