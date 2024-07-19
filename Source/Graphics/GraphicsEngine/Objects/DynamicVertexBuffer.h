#pragma once
#include <vector>
#include "Graphics\GraphicsEngine\GraphicsEngine.h"

struct ID3D11Buffer;

class DynamicVertexBuffer
{
public:
    DynamicVertexBuffer();
    ~DynamicVertexBuffer();

    template<typename VertexType>
    bool CreateBuffer(std::string_view aName, std::vector<VertexType>& aVertexList, size_t aMaxVertexCount);

    template<typename VertexType>
    bool UpdateVertexBuffer(std::vector<VertexType>& aVertexList);

    FORCEINLINE Microsoft::WRL::ComPtr<ID3D11Buffer>& GetVertexBuffer() { return myVertexBuffer; }
    const size_t GetMaxVertexCount() const { return myMaxVertexCount; }
private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
    size_t myMaxVertexCount;
};

template<typename VertexType>
inline bool DynamicVertexBuffer::CreateBuffer(std::string_view aName, std::vector<VertexType>& aVertexList, size_t aMaxVertexCount)
{
    myMaxVertexCount = aMaxVertexCount;
    return GraphicsEngine::Get().CreateDynamicVertexBuffer(aName, aVertexList, myVertexBuffer, aMaxVertexCount);
}

template<typename VertexType>
inline bool DynamicVertexBuffer::UpdateVertexBuffer(std::vector<VertexType>& aVertexList)
{
    return GraphicsEngine::Get().UpdateDynamicVertexBuffer(aVertexList, *this);
}
