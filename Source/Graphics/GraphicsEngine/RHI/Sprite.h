#pragma once
#include <wrl.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "GameEngine/Math/Matrix4x4.hpp"
#include "GameEngine/Math/Vector.hpp"

struct ID3D11Buffer;
struct SpriteVertex;

namespace CU = CommonUtilities;

class Sprite
{
    friend class RenderHardwareInterface;
public:
    Sprite();
    ~Sprite();

    void Initialize(CU::Vector3f aPosition, CU::Vector2f aSize);
    FORCEINLINE const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetVertexBuffer() const { return myVertexBuffer; }
private:
    std::vector<SpriteVertex> myVertices;
    Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
};

