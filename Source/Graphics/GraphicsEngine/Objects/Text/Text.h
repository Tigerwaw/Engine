#pragma once
#include "Font.h"
#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"


#include "Objects/Vertices/TextVertex.h"

struct ID3D11Buffer;
class DynamicVertexBuffer;

class Text
{
public:
    Text();
    ~Text();
    Text(const std::string& aTextContent, std::shared_ptr<Font> aFont, const int aSize);

    struct TextData
    {
        std::vector<TextVertex> vertices;
        std::vector<unsigned> indices;
        unsigned numVertices = 0;
        unsigned numIndices = 0;
        std::shared_ptr<DynamicVertexBuffer> vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
    };

    void Initialize();
    void UpdateBuffers();

    void SetTextContent(const std::string& aTextContent);
    void SetFont(std::shared_ptr<Font> aFont);
    void SetSize(const int aSize);
    void SetPosition(Math::Vector2f aPosition);

    const std::shared_ptr<Texture> GetTexture() const { return myFont->Texture; }
    const TextData& GetTextData() const { return myTextData; }
private:

    std::shared_ptr<Font> myFont;
    TextData myTextData;

    std::string myTextContent;
    Math::Matrix4x4f myMatrix;
    int mySize = 0;
};