#pragma once
#include <memory>
#include "GameEngine\Math\Vector.hpp"
namespace CU = CommonUtilities;

class Sprite;
class Texture;

class SpriteObject
{
public:
    SpriteObject(CU::Vector3f aPosition = CU::Vector3f(), CU::Vector2f aSize = CU::Vector2f());
    ~SpriteObject();

    void SetTexture(std::shared_ptr<Texture> aTexture);
    void SetPosition(CU::Vector3f aPosition);
    void SetSize(CU::Vector2f aSize);

    std::shared_ptr<Sprite> GetSprite() const { return mySprite; }
    std::shared_ptr<Texture> GetTexture() const { return myTexture; }
    const CU::Vector3f GetPosition() const { return myPosition; }
    const CU::Vector2f GetSize() const { return mySize; }
private:
    std::shared_ptr<Sprite> mySprite;
    std::shared_ptr<Texture> myTexture;

    CU::Vector3f myPosition;
    CU::Vector2f mySize;
};