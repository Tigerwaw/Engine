#pragma once
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"


class Texture;
class Material;

class Sprite
{
public:
    Sprite(Math::Vector2f aPosition = Math::Vector2f(), Math::Vector2f aSize = Math::Vector2f(1.0f, 1.0f));
    ~Sprite();

    // Allows a sprite to be rendered with a custom PSO, texture does not need to be set.
    void SetMaterial(std::shared_ptr<Material> aMaterial);

    // Set just a texture and render like a regular sprite.
    void SetTexture(std::shared_ptr<Texture> aTexture);
    void SetPosition(Math::Vector2f aPosition);
    void SetSize(Math::Vector2f aSize);

    std::shared_ptr<Material> GetMaterial() const { return myMaterial; }
    std::shared_ptr<Texture> GetTexture() const { return myTexture; }
    const Math::Matrix4x4f GetMatrix() const { return myMatrix; }
private:
    std::shared_ptr<Material> myMaterial;
    std::shared_ptr<Texture> myTexture;

    Math::Matrix4x4f myMatrix;
};