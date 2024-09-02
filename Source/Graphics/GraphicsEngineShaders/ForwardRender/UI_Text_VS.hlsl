#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Includes/Sprite_Includes.hlsli"

Text_VSout main(TextVertex input)
{
    Text_VSout result;
    result.Position = input.Position / float4(FB_Resolution, 1, 1);
    result.TexCoord = input.TexCoord;
	return result;
}