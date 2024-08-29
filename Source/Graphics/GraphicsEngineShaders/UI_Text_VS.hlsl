#include "Includes/Sprite_Includes.hlsli"

Text_VSout main(TextVertex input)
{
    Text_VSout result;
    result.Position = input.Position;
    result.TexCoord = input.TexCoord;
	return result;
}