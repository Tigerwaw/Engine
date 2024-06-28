#include "Includes/DefaultShaderIncludes.hlsli"

Sprite_VSout main(SpriteVertex input)
{
    Sprite_VSout result;
    result.Position = input.Position;
    result.Size = input.Size;
	return result;
}