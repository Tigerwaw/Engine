#include "Includes/DefaultShaderIncludes.hlsli"
#include "Includes/Sprite_Includes.hlsli"
#include "Includes/ConstantBuffers/SpriteBuffer.hlsli"

Sprite_VSout main()
{
    Sprite_VSout result;
    result.Position = SPB_Position;
    result.Size = SPB_Size;
	return result;
}