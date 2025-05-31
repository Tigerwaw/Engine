#pragma once
#include "Math/Vector.hpp"


#include "Objects/Texture.h"

struct Font
{
	Font();
	~Font();

	struct Atlas
	{
		int Size = 0;
		int Width = 0;
		int Height = 0;
		float EmSize = 0;
		float LineHeight = 0;
		float Ascender = 0;
		float Descender = 0;
		float UnderlineY = 0;
		float UnderlineThickness = 0;
	} Atlas;

	struct Glyph
	{
		char Character = 'x';
		float Advance = 0;
		Math::Vector4f PlaneBounds;
		Math::Vector4f UVBounds;
	};

	std::unordered_map<unsigned, Glyph> Glyphs;
	std::shared_ptr<Texture> Texture;

	Glyph& operator[](char c)
	{
		return Glyphs.at(static_cast<unsigned>(c));
	}
};