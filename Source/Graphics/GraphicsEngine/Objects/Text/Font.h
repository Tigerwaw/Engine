#pragma once
#include "GameEngine/Math/Vector.hpp"
namespace CU = CommonUtilities;

#include "Graphics/GraphicsEngine/Objects/Texture.h"

struct Font
{
	Font();
	~Font();

	struct Atlas
	{
		int Size;
		int Width;
		int Height;
		float EmSize;
		float LineHeight;
		float Ascender;
		float Descender;
		float UnderlineY;
		float UnderlineThickness;
	} Atlas;

	struct Glyph
	{
		char Character;
		float Advance;
		CU::Vector4f PlaneBounds;
		CU::Vector4f UVBounds;
	};

	std::unordered_map<unsigned, Glyph> Glyphs;
	std::shared_ptr<Texture> Texture;

	Glyph& operator[](char c)
	{
		return Glyphs.at(static_cast<unsigned>(c));
	}
};