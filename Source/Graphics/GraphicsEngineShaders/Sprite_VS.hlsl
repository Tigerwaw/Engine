#include "Includes/DefaultShaderIncludes.hlsli"
#include "Includes/Sprite_Includes.hlsli"
#include "Includes/ConstantBuffers/SpriteBuffer.hlsli"

Sprite_VSout main()
{
    Sprite_VSout result;
    result.LocalSpaceMatrix = SPB_Matrix;
    result.IsScreenSpace = SPB_IsScreenSpace;
	return result;
}