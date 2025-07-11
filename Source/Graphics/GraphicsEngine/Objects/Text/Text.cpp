#include "GraphicsEngine.pch.h"
#include "Text.h"
#include "Objects/DynamicVertexBuffer.h"

Text::Text() = default;
Text::~Text() = default;

Text::Text(const std::string& aTextContent, std::shared_ptr<Font> aFont, const int aSize)
{
	SetTextContent(aTextContent);
	SetFont(aFont);
	SetSize(aSize);
}

void Text::Initialize()
{
	myTextData.vertexBuffer = std::make_shared<DynamicVertexBuffer>();
	myTextData.vertexBuffer->CreateBuffer("Text Vertex Buffer", myTextData.vertices, myTextData.vertices.size());
	GraphicsEngine::Get().GetResourceVendor().CreateIndexBuffer("Text Index Buffer", myTextData.indices, myTextData.indexBuffer, true);
}

void Text::UpdateBuffers()
{
	if (!myTextData.vertexBuffer)
	{
		Initialize();
	}

	myTextData.vertexBuffer->UpdateVertexBuffer(myTextData.vertices);
	GraphicsEngine::Get().UpdateDynamicIndexBuffer(myTextData.indices, myTextData.indexBuffer);
}

void Text::SetTextContent(const std::string& aTextContent)
{
	myTextContent = aTextContent;
	float X = 0;
	float scalarOffset = 0.65f;

	for (char c : myTextContent)
	{
		Font::Glyph& glyph = myFont->operator[](c);

		const int fontSize = myFont->Atlas.Size * mySize;
		const float charAdvance = glyph.Advance * fontSize;
		const unsigned currentVertexCount = static_cast<unsigned>(myTextData.vertices.size());
		Math::Vector4f bounds = glyph.UVBounds;
		Math::Vector4f offsets = glyph.PlaneBounds;

		offsets.x = glyph.PlaneBounds.x * charAdvance;
		offsets.z = glyph.PlaneBounds.z * fontSize * scalarOffset;
		offsets.y = glyph.PlaneBounds.y * charAdvance;
		offsets.w = glyph.PlaneBounds.w * fontSize;

		if (abs(offsets.w) > 0.005f)
		{
			offsets.w += myFont->Atlas.Descender * fontSize;
		}

		myTextData.vertices.emplace_back(myMatrix(4, 1) + X + offsets.x, myMatrix(4, 2) + offsets.y, bounds.x, 1 - bounds.y);
		myTextData.vertices.emplace_back(myMatrix(4, 1) + X + offsets.x, myMatrix(4, 2) + offsets.w, bounds.x, 1 - bounds.w);
		myTextData.vertices.emplace_back(myMatrix(4, 1) + X + offsets.z, myMatrix(4, 2) + offsets.y, bounds.z, 1 - bounds.y);
		myTextData.vertices.emplace_back(myMatrix(4, 1) + X + offsets.z, myMatrix(4, 2) + offsets.w, bounds.z, 1 - bounds.w);

		X += charAdvance + scalarOffset;

		myTextData.indices.emplace_back(currentVertexCount);
		myTextData.indices.emplace_back(currentVertexCount + 1);
		myTextData.indices.emplace_back(currentVertexCount + 2);
		myTextData.indices.emplace_back(currentVertexCount + 1);
		myTextData.indices.emplace_back(currentVertexCount + 3);
		myTextData.indices.emplace_back(currentVertexCount + 2);
	}

	myTextData.numVertices = static_cast<unsigned>(myTextData.vertices.size());
	myTextData.numIndices = static_cast<unsigned>(myTextData.indices.size());

	UpdateBuffers();
}

void Text::SetFont(std::shared_ptr<Font> aFont)
{
	myFont = aFont;
}

void Text::SetSize(const int aSize)
{
	mySize = aSize;
}

void Text::SetPosition(Math::Vector2f aPosition)
{
	myMatrix(4, 1) = aPosition.x;
	myMatrix(4, 2) = aPosition.y;
}
