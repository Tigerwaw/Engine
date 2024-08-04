#pragma once
#include <memory>
#include "GameEngine\Math\Vector.hpp"
#include "GameEngine\Math\Matrix.hpp"
namespace CU = CommonUtilities;

class Texture;
class Material;

class Sprite
{
public:
    Sprite(CU::Vector2f aPosition = CU::Vector2f(), CU::Vector2f aSize = CU::Vector2f());
    ~Sprite();

    // Allows a sprite to be rendered with a custom PSO, texture does not need to be set.
    void SetMaterial(std::shared_ptr<Material> aMaterial);

    // Set just a texture and render like a regular sprite.
    void SetTexture(std::shared_ptr<Texture> aTexture);
    void SetScreenspacePosition(CU::Vector2f aPosition);
    void SetScreenspaceSize(CU::Vector2f aSize);
    void SetIsScreenspace(bool aIsScreenspace);

    std::shared_ptr<Material> GetMaterial() const { return myMaterial; }
    std::shared_ptr<Texture> GetTexture() const { return myTexture; }
    const CU::Matrix4x4f GetScreenspaceMatrix() const { return myScreenspaceMatrix; }
    const bool GetIsScreenspace() const { return myIsScreenspace; }
private:
    std::shared_ptr<Material> myMaterial;
    std::shared_ptr<Texture> myTexture;

    CU::Matrix4x4f myScreenspaceMatrix;
    bool myIsScreenspace;
};