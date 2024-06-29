#pragma once
#include <wrl.h>
#include <vector>

struct ID3D11Buffer;
struct SpriteVertex;

class Sprite
{
    friend class RenderHardwareInterface;
public:
    Sprite();
    ~Sprite();
    FORCEINLINE const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetVertexBuffer() const { return myVertexBuffer; }
private:
    std::vector<SpriteVertex> myVertices;
    Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
};

