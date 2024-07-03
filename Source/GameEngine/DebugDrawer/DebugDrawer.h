#pragma once
#include <vector>
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

struct Line
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
private:

    std::vector<Line> myLines;
};

