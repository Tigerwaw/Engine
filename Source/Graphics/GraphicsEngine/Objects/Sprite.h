#pragma once
#include <memory>
#include "GameEngine\Math\Vector.hpp"
namespace CU = CommonUtilities;

class Texture;

class Sprite
{
public:
    Sprite(CU::Vector3f aPosition = CU::Vector3f(), CU::Vector2f aSize = CU::Vector2f());
    ~Sprite();

    void SetTexture(std::shared_ptr<Texture> aTexture);
    void SetPosition(CU::Vector3f aPosition);
    void SetSize(CU::Vector2f aSize);

    std::shared_ptr<Texture> GetTexture() const { return myTexture; }
    const CU::Vector3f GetPosition() const { return myPosition; }
    const CU::Vector2f GetSize() const { return mySize; }
private:
    std::shared_ptr<Texture> myTexture;

    CU::Vector3f myPosition;
    CU::Vector2f mySize;
};